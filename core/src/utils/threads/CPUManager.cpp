#include <algorithm>
#include <iostream>
#include <mutex>
#include <utils/logging/logger.h>
#include <utils/misc_utils.h>
#include <utils/threads/CPUManager.h>

constexpr const char* online_cores_sys_file_path = "/sys/devices/system/cpu/online";
constexpr const char* isolated_cores_sys_file_path = "/sys/devices/system/cpu/isolated";

pt::threads::CPUManager::CPUManager() : Singleton(), logger("CPUManager") {
    parse_cpus();
    utils::logging::initialize_dawglogger();
}

void pt::threads::CPUManager::parse_cpus() {
    std::string unparsed_cores;
    std::ifstream core_details_f;

    core_details_f.open(online_cores_sys_file_path, std::ios::in);
    std::getline(core_details_f, unparsed_cores);
    core_details_f.close();
    pinned_core_pool = utils::parse_numeric_ranges(unparsed_cores);
    online_cores = utils::parse_numeric_ranges(unparsed_cores);

    core_details_f.open(isolated_cores_sys_file_path, std::ios::in);
    std::getline(core_details_f, unparsed_cores);
    core_details_f.close();
    isol_core_pool = utils::parse_numeric_ranges(unparsed_cores);

    for (auto cpu : isol_core_pool) {
        pinned_core_pool.erase(
            std::ranges::find(pinned_core_pool.begin(), pinned_core_pool.end(), cpu));
    }
}

std::vector<pt::threads::Core>
pt::threads::CPUManager::allocate(const ThreadPolicy& policy) {
    auto manager = getInstance();
    std::lock_guard lock(manager->mutex_);

    size_t total_cpus = manager->pinned_core_pool.size();
    if (policy.affinity_type == AffinityType::ISOLATED) {
        total_cpus += manager->isol_core_pool.size();
    }

    if (policy.cores > total_cpus) {
        manager->logger.throw_error<std::runtime_error>(LOG_SRC,
                                                        "Not enough resources to allocate {} cpus.",
                                                        policy.cores);
    }
    std::vector<Core> allocated_cpus;
    allocated_cpus.reserve(policy.cores);

    auto pinned_core_pool_copy = manager->pinned_core_pool;
    auto isol_core_pool_copy = manager->isol_core_pool;
    auto original_pinned_core_pool_size = manager->pinned_core_pool.size();

    switch (policy.affinity_type) {
        case AffinityType::NORMAL:
            for (size_t i = 0; i < policy.cores; ++i) {
                allocated_cpus.emplace_back(pinned_core_pool_copy.at(i), AffinityType::NORMAL);
            }
            break;
        case AffinityType::PINNED:
            for (size_t i = 0; i < policy.cores; ++i) {
                allocated_cpus.emplace_back(pinned_core_pool_copy.back(), AffinityType::PINNED);
                pinned_core_pool_copy.pop_back();
            }
            break;
        case AffinityType::ISOLATED:
            size_t isol_cpus_to_allocate = std::min(policy.cores, isol_core_pool_copy.size());
            size_t pinned_cpus_to_allocate = policy.cores - isol_cpus_to_allocate;

            if (isol_cpus_to_allocate < policy.cores) {
                if (policy.strict) {
                    manager->logger.throw_error<std::runtime_error>(LOG_SRC, "Not enough resources to allocate {} isolated cpus.", policy.cores);
                }

                allocated_cpus = allocate({policy.thread_name,
                                                     pinned_cpus_to_allocate,
                                                     AffinityType::PINNED,
                                                     policy.strict,
                                                     policy.priority});

                manager->logger.info(
                    LOG_SRC,
                    "An isolated core was requested, but a pinned one was allocated instead.");

                pinned_core_pool_copy = manager->pinned_core_pool;
                isol_core_pool_copy = manager->isol_core_pool;
            }

            for (size_t i = 0; i < isol_cpus_to_allocate; ++i) {
                allocated_cpus.emplace_back(isol_core_pool_copy.back(), AffinityType::ISOLATED);
                isol_core_pool_copy.pop_back();
            }
            break;
    }

    manager->pinned_core_pool = std::move(pinned_core_pool_copy);
    manager->isol_core_pool = std::move(isol_core_pool_copy);

    if (original_pinned_core_pool_size != 0 && manager->pinned_core_pool.empty()) [[unlikely]] {
        manager->logger.notice(LOG_SRC,
                               "The CPUManager has exhausted its available CPUs. Attemping "
                               "to allocate more will fail.");
    }

    return allocated_cpus;
}

void pt::threads::CPUManager::deallocate(Core core) {
    auto manager = getInstance();

    if (std::ranges::find(manager->online_cores, core.cpu_id) == manager->online_cores.end()) {
        manager->logger.notice(LOG_SRC,
                               "Attempted deallocation of a core that doesn't exist ({})",
                               core.cpu_id);
    }

    switch (core.affinity_type) {
        case AffinityType::NORMAL:
            // Normal cores shouldn't be returned to the pool, as they weren't taken out.
            break;
        case AffinityType::ISOLATED:
            if (std::ranges::find(manager->isol_core_pool, core.cpu_id) !=
                manager->isol_core_pool.end()) {
                manager->logger.warning(LOG_SRC,
                                        "Attempted to deallocate a core that was not allocated.");
            }
            manager->isol_core_pool.emplace_back(core.cpu_id);
            break;
        case AffinityType::PINNED:
            if (std::ranges::find(manager->pinned_core_pool, core.cpu_id) !=
                manager->pinned_core_pool.end()) {
                manager->logger.warning(LOG_SRC,
                                        "Attempted to deallocate a core that was not allocated.");
            }
            manager->pinned_core_pool.emplace_back(core.cpu_id);
            break;
    }
}

void pt::threads::CPUManager::deallocate(const std::vector<Core>& cores) {
    for (auto core : cores) {
        deallocate(core);
    }
}
