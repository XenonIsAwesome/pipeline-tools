#include "utils/utils.h"

#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <utils/threads/CPUManager.h>

static constexpr const char* ONLINE_CPUS_SYS_FILE_PATH = "/sys/devices/system/cpu/online";
static constexpr const char* ISOLATED_CPUS_SYS_FILE_PATH = "/sys/devices/system/cpu/isolated";

std::vector<pt::threads::Core> pt::threads::CPUManager::allocate(ThreadPolicy policy) {
    if (policy.cores == 0) {
        return {};
    }

    std::vector<Core> allocated_cores;
    auto manager = CPUManager::getInstance();

    if (policy.cores > manager->online_cores.size()) {
        throw std::runtime_error("Not enough online cores to allocate");
    }

    std::vector<int> pinned = manager->pinned_core_pool;
    std::vector<int> isolated = manager->isolated_core_pool;

    switch (policy.affinity_type) {
        case AffinityType::NORMAL:
        case AffinityType::PINNED:
            if (policy.cores > pinned.size()) {
                throw std::runtime_error("Not enough non-isolated cores to allocate");
            }

            for (size_t i = 0; i < policy.cores; i++) {
                allocated_cores.emplace_back(pinned.back(), policy.affinity_type);
                pinned.pop_back();
            }

            if (policy.affinity_type == AffinityType::NORMAL) {
                return allocated_cores;
            }
            break;
        case AffinityType::ISOLATED:
            if (policy.strict && policy.cores > isolated.size()) {
                throw std::runtime_error("Not enough isolated cores to allocate");
            }

            auto isolated_cores_to_alloc = std::min(policy.cores, isolated.size());
            auto pinned_cores_to_alloc = policy.cores - isolated_cores_to_alloc;

            // Allocating ISOLATED cores
            for (size_t i = 0; i < isolated_cores_to_alloc; i++) {
                allocated_cores.emplace_back(isolated.back(), policy.affinity_type);
                isolated.pop_back();
            }
            manager->isolated_core_pool = std::move(isolated);

            // Allocating the rest as PINNED
            if (pinned_cores_to_alloc > 0) {
                std::cout << "[CPUManager]:WARNING: Not enough isolated cores to allocate, "
                             "allocating pinned cores instead." << std::endl;

                auto pinned_allocated_cores = CPUManager::allocate({
                    policy.thread_name,
                    pinned_cores_to_alloc,
                    AffinityType::PINNED,
                    policy.strict,
                    policy.priority});

                for (auto& core: pinned_allocated_cores) {
                    allocated_cores.push_back(core);
                }
            }

            return allocated_cores;
    }

    manager->pinned_core_pool = std::move(pinned);
    manager->isolated_core_pool = std::move(isolated);

    return allocated_cores;
}

void pt::threads::CPUManager::deallocate(const Core& core) {
    auto manager = getInstance();

    if (std::ranges::find(manager->online_cores, core.cpu_id) == manager->online_cores.end()) {
        std::cout << "[CPUManager::deallocate]:WARNING: Core with cpu_id " << core.cpu_id
            << " is not online!" << std::endl;
    }

    std::vector<int>& pool = manager->pinned_core_pool;
    switch (core.affinity_type) {
        case AffinityType::NORMAL:
            return;
        case AffinityType::PINNED:
            pool = manager->pinned_core_pool;
            break;
        case AffinityType::ISOLATED:
            pool = manager->isolated_core_pool;
            break;
    }

    if (std::ranges::find(pool, core.cpu_id) != pool.end()) {
        std::cout << "[CPUManager::deallocate]:WARNING: Core with cpu_id " << core.cpu_id
            << " was not allocated." << std::endl;
    }
    pool.push_back(core.cpu_id);
}

void pt::threads::CPUManager::deallocate(const std::vector<Core>& cores) {
    for (const Core& core : cores) {
        deallocate(core);
    }
}

void pt::threads::CPUManager::parse_cores() {
    auto reader = [](const std::filesystem::path& filepath) {
        std::ifstream file;
        file.open(filepath, std::ios::in);

        std::string line;
        std::getline(file, line);

        return line;
    };

    auto unparsed_online_cores = reader(ONLINE_CPUS_SYS_FILE_PATH);
    auto unparsed_isolated_cores = reader(ISOLATED_CPUS_SYS_FILE_PATH);

    online_cores = utils::parse_numeric_range(unparsed_online_cores);
    isolated_core_pool = utils::parse_numeric_range(unparsed_isolated_cores);

    pinned_core_pool = online_cores;
    for (auto& core: isolated_core_pool) {
        auto it = std::ranges::find(pinned_core_pool, core);
        pinned_core_pool.erase(it);
    }
}

void pt::threads::CPUManager::print_cores() const {
    std::cout << "pinned: ";
    for (auto core: pinned_core_pool) {
        std::cout << core << ", ";
    }
    std::cout << std::endl;

    std::cout << "isolated: ";
    for (auto core: isolated_core_pool) {
        std::cout << core << ", ";
    }
    std::cout << std::endl;
}