#pragma once

#include <dawg-log/logger.hpp>
#include <utils/logging/logger.h>
#include <utils/singleton/Singleton.h>
#include <utils/threads/ThreadPolicy.h>
#include <vector>

namespace pt::threads {
/**
 * A singleton class for core allocation management
 * used to allocate pinned and isolated cores for use in pt.
 */
class CPUManager : public utils::Singleton<CPUManager> {
public:
    CPUManager();

    /**
     * Allocates cpus based on the availabe cpus and isolated ones.
     *
     * \image html CPUManager_allocate_behaviour.png
     *
     * @param policy The amount of each cpu type (isolated or not) to allocate.
     *
     * @throw std::runtime_error: When not enough cores to allocate
     */
    static std::vector<Core> allocate(const ThreadPolicy& policy);

    /**
     * Retreives a cpu to the correct cpu pool.
     * @param core The cpu to return to the pool of availabe cpus
     */
    static void deallocate(Core core);

    /**
     * Uses `CPUManager::deallocate(cpu_pair)` on each item of the vector.
     * @param cores A vector of cpu
     */
    static void deallocate(const std::vector<Core>& cores);

private:
    /**
     * Utility function that parses files in `/sys` to figure out how many cpus are there, and which ones are isolated.
     *
     * @note This function is very dependant on the linux filesystem,
     *       if pt is ever moved to anything else that isn't linux, this will need to be changed.
     */
    void parse_cpus();

    std::vector<int> pinned_core_pool; /**< The pool of available non-isolated cpus */
    std::vector<int> isol_core_pool; /**< The pool of available isolated cpus */
    std::vector<int> online_cores; /**< The list of online cores, used for validation in allocate. */
    std::mutex mutex_;
    DawgLog::TaggedLogger logger {"CPUManager"};
};
}  // namespace pt::threads