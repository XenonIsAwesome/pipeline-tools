#pragma once

#include <string>

namespace pt::threads {

#define CHOSEN_SCHEDULER_ALGO (SCHED_FIFO)

/**
 * pthread priority scales for ease of use (a value between 0 and 99).
 */
#define THREAD_PRIORITY_LOWEST  (sched_get_priority_min(CHOSEN_SCHEDULER_ALGO))
#define THREAD_PRIORITY_HIGHEST (sched_get_priority_max(CHOSEN_SCHEDULER_ALGO))
#define THREAD_PRIORITY_NORMAL  ((THREAD_PRIORITY_HIGHEST - THREAD_PRIORITY_LOWEST) / 2)
#define THREAD_PRIORITY_LOW     = ((THREAD_PRIORITY_NORMAL - THREAD_PRIORITY_LOWEST) / 2)
#define THREAD_PRIORITY_HIGH    ((THREAD_PRIORITY_HIGHEST - THREAD_PRIORITY_NORMAL) / 2)

using ThreadPriority = int;

/**
 * The type of allocation requested
 */
enum class AffinityType {
    NORMAL, /**< A core that isn't pinned or isolated */
    PINNED, /**< A core pinned by pt itself */
    ISOLATED /**< A core isolated by the system */
};

/*
 * A policy used to allocate cores
 */
struct ThreadPolicy {
    std::string thread_name; /**< Thread name (optional) */

    size_t cores = 0; /**< The amount of cpus requesed */

    /** The type of allocation requested (pinned or isolated) */
    AffinityType affinity_type = AffinityType::NORMAL;

    /** When strict mode is enabled, CPUManager will raise errors when allocating */
    bool strict = false;

    ThreadPriority priority =
        THREAD_PRIORITY_NORMAL; /**< Priority of the thread (value between 0-99) */
};

/**
 * A data structure that contains the id cpu and the type of it (pinned or isolated).
 */
struct Core {
    int cpu_id;
    AffinityType affinity_type;
};
}  // namespace pt::threads