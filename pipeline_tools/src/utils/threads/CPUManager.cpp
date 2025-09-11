#include "utils/utils.h"


#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <utils/threads/CPUManager.h>

static constexpr std::filesystem::path ONLINE_CPUS_SYS_FILE_PATH = "/sys/devices/system/cpu/online";
static constexpr std::filesystem::path ISOLATED_CPUS_SYS_FILE_PATH = "/sys/devices/system/cpu/isolated";

std::vector<pt::threads::Core> pt::threads::CPUManager::allocate(ThreadPolicy policy) {
    // TODO: implement
    return;
}

void pt::threads::CPUManager::deallocate(Core core) {
    // TODO: implement
}

void pt::threads::CPUManager::deallocate(const std::vector<Core> &cores) {
    for (const auto& core : cores) {
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