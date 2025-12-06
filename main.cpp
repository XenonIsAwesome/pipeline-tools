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
}
