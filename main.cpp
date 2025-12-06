#include <utils/ipc/rmq/utils.h>
#include <utils/threads/CPUManager.h>
#include <iostream>
#include <utils/threads/Worker.h>

#include <utils/ipc/rmq/Publisher.hpp>
#include <utils/ipc/rmq/Consumer.hpp>
#include <modules/math/SumAggregator.hpp>

int main() {
    std::cout << "Hello World!" << std::endl;

    while (true) {
        pt::ipc::rmq::publish_message_to_queue<std::string>("Hello World", "pt_test");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
