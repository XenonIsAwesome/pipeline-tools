#pragma once

#include <pthread.h>
#include <string>

namespace pt::threads {

// Affinity
enum class AffinityType { NORMAL, PINNED, ISOLATED };

// Priority
using ThreadPriority = int;
constexpr int CHOSEN_SCHED_ALGO = SCHED_FIFO;

#define THREAD_PRIORITY_HIGHEST (sched_get_priority_max(CHOSEN_SCHED_ALGO))
#define THREAD_PRIORITY_LOWEST (sched_get_priority_min(CHOSEN_SCHED_ALGO))
#define THREAD_PRIORITY_NORMAL ((THREAD_PRIORITY_HIGHEST - THREAD_PRIORITY_LOWEST) / 2)
#define THREAD_PRIORITY_HIGH ((THREAD_PRIORITY_HIGHEST - THREAD_PRIORITY_NORMAL) / 2)
#define THREAD_PRIORITY_LOW ((THREAD_PRIORITY_NORMAL - THREAD_PRIORITY_LOWEST) / 2)

struct ThreadPolicy {
    std::string thread_name;

    size_t cores;
    AffinityType affinity_type = AffinityType::NORMAL;

    bool strict = false;

    ThreadPriority priority = THREAD_PRIORITY_NORMAL;
};

struct Core {
    int cpu_id;
    AffinityType affinity_type;
};
}