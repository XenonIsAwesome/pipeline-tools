#pragma once

#include <vector>
#include <utils/Singleton.hpp>
#include <utils/threads/ThreadPolicy.h>

namespace pt::threads {
class CPUManager final: public utils::Singleton<CPUManager> {
public:
    CPUManager() { parse_cores(); }

    static std::vector<Core> allocate(ThreadPolicy policy);

    static void deallocate(const Core& core);
    static void deallocate(const std::vector<Core>& cores);

    void print_cores();

private:
    void parse_cores();

    std::vector<int> online_cores;
    std::vector<int> isolated_core_pool;
    std::vector<int> pinned_core_pool;
};
}