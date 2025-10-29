#include "pt/core/utils/ipc/rmq/utils.h"
#include "pt/core/utils/threads/CPUManager.h"
#include <iostream>
#include <pt/core/utils/threads/Worker.h>

#include <pt/core/utils/ipc/rmq/Publisher.hpp>
#include <pt/core/utils/ipc/rmq/Consumer.hpp>

int main() {
    while (true) {
        pt::ipc::rmq::publish_message_to_queue<std::string>("Hello World", "pt_test");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // pt::threads::Worker wa({.cores = 2, .affinity_type = pt::threads::AffinityType::PINNED}, [](std::atomic_flag& flag) {
    //     while (flag.test()) {
    //         std::cout << "A" << std::endl;
    //     }
    // });
    //
    // pt::threads::Worker wb({.cores = 2, .affinity_type = pt::threads::AffinityType::NORMAL}, [](std::atomic_flag& flag) {
    //     while (flag.test()) {
    //         std::cout << "B" << std::endl;
    //     }
    // });
    //
    // pt::threads::CPUManager::getInstance()->print_cores();
    //
    // wa.start();
    // wb.start();
    //
    // pt::threads::CPUManager::getInstance()->print_cores();
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    //
    // wa.stop();
    // wb.stop();
    //
    // pt::threads::CPUManager::getInstance()->print_cores();

}
