#include <utils/queues/LockFreeQueue.hpp>
#include <utils/threads/CPUManager.h>


#include <iostream>
#include <utils/threads/Worker.h>

int main() {
    pt::threads::Worker wa({.cores = 2, .affinity_type = pt::threads::AffinityType::PINNED}, [](std::atomic_flag& flag) {
        while (flag.test()) {
            std::cout << "A" << std::endl;
        }
    });

    pt::threads::Worker wb({.cores = 2, .affinity_type = pt::threads::AffinityType::NORMAL}, [](std::atomic_flag& flag) {
        while (flag.test()) {
            std::cout << "B" << std::endl;
        }
    });

    pt::threads::CPUManager::getInstance()->print_cores();

    wa.start();
    wb.start();

    pt::threads::CPUManager::getInstance()->print_cores();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    wa.stop();
    wb.stop();

    pt::threads::CPUManager::getInstance()->print_cores();
}
