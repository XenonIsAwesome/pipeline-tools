#pragma once

#include "concepts.h"
#include "MQEntity.h"
#include "Publisher.hpp"
#include <config/get_env.h>

namespace pt::ipc::rmq {
template<concepts::RMQSerializer T>
void publish_message_to_queue(T message, const std::string& queue, std::string exchange = "") {
    if (exchange.empty()) {
        exchange = config::get_env<std::string>("RABBITMQ_DEFAULT_EXCHANGE", "pt");
    }

    RMQOptions options {
        .exchange_name = exchange,
        .queue_name = queue
    };

    Publisher<T> pub(options);
    pub.publish(message);
}
}
