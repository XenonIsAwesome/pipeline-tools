#include <iostream>
#include <utils/threads/CPUManager.h>
#include <utils/threads/Worker.h>

void pt::threads::Worker::allocate_cores(){
    std::vector<Core> cores = CPUManager::allocate(policy);
    if (cores.empty()) {
        throw std::runtime_error("CPUManager couldn't allocate!");
    }

    allocated_cores = std::move(cores);
}

bool pt::threads::Worker::set_affinity() const {
    cpu_set_t mask;
    CPU_ZERO(&mask);

    for (auto& core: allocated_cores) {
        CPU_SET(core.cpu_id, &mask);
    }

    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask) == 0;
}

bool pt::threads::Worker::set_priority() const {
    sched_param params{};
    params.sched_priority = policy.priority;

    return pthread_setschedparam(pthread_self(), SCHED_FIFO, &params) == 0;
}

bool pt::threads::Worker::set_name() const{
    if (policy.thread_name.empty()) {
        return true;
    }
    return pthread_setname_np(pthread_self(), policy.thread_name.c_str()) == 0;
}

void pt::threads::Worker::start() {
    allocate_cores();

    stop_flag.clear();
    worker_thread = std::thread([this]() {
        if (!set_affinity()) {
            throw std::runtime_error("Failed to set affinity! Are running with privileges?");
        }

        if (!set_priority()) {
            throw std::runtime_error("Failed to set priority! Are running with privileges?");
        }

        if (!set_name()) {
            std::cout << "[Worker]:WARNING: Couldn't set thread name" << std::endl;
        }

        func(stop_flag);
    });
}

void pt::threads::Worker::stop(){
    stop_flag.test_and_set(std::memory_order_relaxed);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (worker_thread.joinable()) {
        worker_thread.join();
    } else {
        std::cout << "[Worker]:WARNING: Couldn't join the thread." << std::endl;
    }

    CPUManager::deallocate(allocated_cores);
    allocated_cores = {};
}

