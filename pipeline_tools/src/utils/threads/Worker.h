#pragma once

#include <utils/threads/ThreadPolicy.h>
#include <atomic>
#include <functional>
#include <thread>

namespace pt::threads {struct Core;
class Worker {
public:
    using func_t = std::function<void(std::atomic_flag&)>;

    Worker(ThreadPolicy&& policy, func_t&& func):
        policy(std::move(policy)), func(std::move(func)) {}

    Worker(const Worker& other):
        policy(other.policy), func(other.func) {}

    Worker(const Worker&& other) noexcept:
        policy(std::move(other.policy)), func(std::move(other.func)) {}

    void start();

    void stop();

private:
    void allocate_cores();

    bool set_affinity();
    bool set_priority() const;
    bool set_name() const;

    ThreadPolicy policy;
    func_t func;

    std::vector<Core> allocated_cores;

    std::thread worker_thread;
    std::atomic_flag stop_flag{ATOMIC_FLAG_INIT};
};
}