#include "CPUManager.h"


#include <utils/threads/Worker.h>

void pt::threads::Worker::allocate_cores(){
    CPUManager::getInstance();

    std::vector<Core> cores;
    try {
        cores = CPUManager::allocate(policy);
    } catch (std::runtime_error& e) {
        // TODO: warn / throw
    }

    if (cores.empty()) {
        // TODO: throw
    }

    allocated_cores = std::move(cores);
}

bool pt::threads::Worker::set_affinity() {
    allocate_cores();

    cpu_set_t mask;
    CPU_ZERO(&mask);

    for (auto& core: allocated_cores) {
        CPU_SET(core.cpu_id, &mask);
    }

    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask) == 0;
}

bool pt::threads::Worker::set_priority() const {
    sched_param params;
    params.sched_priority = policy.priority;

    return pthread_setschedparam(pthread_self(), SCHED_FIFO, &params) == 0;
}

bool pt::threads::Worker::set_name(){
    if (policy.thread_name.empty()) {
        return true;
    }
    return pthread_setname_np(pthread_self(), policy.thread_name.c_str()) == 0;
}



void pt::threads::Worker::start(){
    worker_thread = std::thread([this]() {
        if (!set_affinity()) {
            // TODO: throw
        }

        if (!set_priority()) {
            // TODO: throw
        }

        if (!set_name()) {
            // TODO: warn
        }

        func(stop_flag);
    });
}

void pt::threads::Worker::stop(){
    stop_flag.notify_all();

    if (worker_thread.joinable()) {
        worker_thread.join();
    } else {
        // TODO: throw / warn
    }

    CPUManager::getInstance();
    CPUManager::deallocate(allocated_cores);
}

