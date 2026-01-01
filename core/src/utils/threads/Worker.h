#pragma once

#include <functional>
#include <thread>
#include <atomic>
#include <utils/threads/CPUManager.h>

namespace pt::threads {
enum class WorkerState { Idle, Starting, Started, Stopping, Stopped };

/**
 * Manages a single thread.
 */
class Worker {
public:
    using func_t = std::function<void(std::atomic_flag&)>;

    Worker(ThreadPolicy&& policy, func_t&& func) :
        logger("Worker-" + policy.thread_name), policy(std::move(policy)), func(std::move(func)) {}

    Worker(Worker&& other) noexcept :
        logger("Worker-" + policy.thread_name), policy(std::move(other.policy)),
        func(std::move(other.func)) {}

    Worker(const Worker& other) noexcept :
        logger("Worker-" + policy.thread_name), policy(other.policy), func(other.func) {}

    ~Worker() {
        stop();
    }

    /**
     * Starts the thread and sets the priority, affinity and name
     */
    void start(bool strict = true);

    /**
     * Stops the thread
     */
    void stop();

private:
    /**
     * Helper function to set the affinity
     * @param mask which cpus to use
     * @return flag if worked
     */
    [[nodiscard]] bool set_affinity(const cpu_set_t& mask) const;

    /**
     * Helper function to set the thread priority
     * @return flag if worked
     */
    [[nodiscard]] bool set_priority() const;

    /**
     * Helper function to set the name of the thread (for debugging purposes)
     * @return flag if worked
     */
    [[nodiscard]] bool set_name() const;

    void worker_target(bool strict, const cpu_set_t& mask);

    /**
     * Allocates the cores from the CPU manager and puts in `mask`
     * @param mask The var that gets the allocated cores
     * @return flag if worked
     */
    bool allocate_cores(cpu_set_t& mask);

    ThreadPolicy policy;
    func_t func;
    std::vector<Core> allocated_cores;
    std::jthread work_thread;
    std::atomic<WorkerState> state{WorkerState::Idle};

    /// The worker func gets this variable to stop gracefully
    std::atomic_flag stop_flag;
    DawgLog::TaggedLogger logger;
};
}  // namespace pt::threads
