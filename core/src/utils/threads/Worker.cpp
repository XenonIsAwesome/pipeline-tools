#include <utils/threads/Worker.h>
#include <iostream>

bool pt::threads::Worker::allocate_cores(cpu_set_t& mask) {
    CPUManager::getInstance();

    std::vector<Core> cores;
    cores.reserve(policy.cores);
    try {
        cores = CPUManager::allocate(policy);
    } catch (std::runtime_error) {
        CPUManager::deallocate(cores);
        return false;
    }

    if (cores.empty()) {
        logger.throw_error<std::runtime_error>(LOG_SRC, "CPUManager failed allocating");
    }

    CPU_ZERO(&mask);

    for (auto core : cores) {
        CPU_SET(core.cpu_id, &mask);
    }

    allocated_cores = cores;
    return true;
}


bool pt::threads::Worker::set_affinity(const cpu_set_t& mask) const {
    if (policy.cores == 0) {
        return true;
    }
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &mask) == 0;
}

bool pt::threads::Worker::set_priority() const {
    sched_param param;
    param.sched_priority = policy.priority;

    return pthread_setschedparam(pthread_self(), CHOSEN_SCHEDULER_ALGO, &param) == 0;
}

bool pt::threads::Worker::set_name() const {
    if (policy.thread_name.empty()) {
        return true;
    }
    return pthread_setname_np(pthread_self(), policy.thread_name.c_str()) == 0;
}

void pt::threads::Worker::worker_target(bool strict, const cpu_set_t& mask) {
    if (!set_priority()) {
        logger.error(LOG_SRC, "Couldn't set priority, are you running with privileges?");
        if (strict) {
            logger.throw_error<std::runtime_error>(
                LOG_SRC,
                "Couldn't set priority, are you running with privileges?");
        }
    }

    if (!set_affinity(mask)) {
        logger.error(LOG_SRC, "Couldn't set affinity, are you running with privileges?");
        if (strict) {
            logger.throw_error<std::runtime_error>(
                LOG_SRC,
                "Couldn't set affinity, are you running with privileges?");
        }
    }

    if (!set_name()) {
        logger.warning(LOG_SRC, "Couldn't set name");
    }

    this->func(this->stop_flag);
}


void pt::threads::Worker::start(bool strict) {
    stop_flag.clear();

    cpu_set_t mask;
    allocate_cores(mask);

    work_thread = std::jthread([&strict, &mask, this] {
        worker_target(strict, mask);
    });
}

void pt::threads::Worker::stop() {
    stop_flag.test_and_set(std::memory_order_relaxed);
    stop_flag.notify_all();

    CPUManager::deallocate(allocated_cores);
    allocated_cores = {};
}