#pragma once

#include "Publisher.h"

#include <sstream>


template<pt::ipc::rmq::concepts::RMQSerialized T>
pt::ipc::rmq::Publisher<T>::Publisher(const RMQOptions& options, const std::string& routing_key) :
    MQEntity(options, "Publisher") {
    if (routing_key.empty()) {
        this->routing_key = options.queue_name;
    } else {
        this->routing_key = routing_key;
    }

    bool exchange_exists;
    bool queue_exists;

    try {
        exchange_exists = channel->CheckExchangeExists(options.exchange_name);
    } catch(std::exception) {
        exchange_exists = false;
    }

    if (!exchange_exists) {
        channel->DeclareExchange(options.exchange_name,
                                 options.exchange_type,
                                 options.exchange_passive,
                                 options.exchange_durable,
                                 options.exchange_auto_delete);
    }

    try {
        queue_exists = channel->CheckQueueExists(options.queue_name);
    } catch(std::exception) {
        queue_exists = false;
    }

    if (!queue_exists) {
        channel->DeclareQueue(options.queue_name,
                              options.queue_passive,
                              options.queue_durable,
                              options.queue_exclusive,
                              options.queue_auto_delete);
    }
}

template<pt::ipc::rmq::concepts::RMQSerialized T>
void pt::ipc::rmq::Publisher<T>::publish(T value, bool mandatory, bool immediate) {
    auto message = AmqpClient::BasicMessage::Create();

    if constexpr (concepts::JSONSerialized<T>) {
        message->ContentType("application/json");

        nlohmann::json json_doc = value.serialize();
        message->Body(json_doc.dump());
    } else if constexpr (concepts::StringSerialized<T>) {
        message->Body(value.serialize());
    } else if constexpr (std::is_same_v<T, nlohmann::json>) {
        message->ContentType("application/json");
        message->Body(value.dump());
    } else if constexpr (std::is_same_v<T, std::string>) {
        message->Body(value);
    }

    try {
        this->channel->BasicPublish(options.exchange_name,
                                    routing_key,
                                    std::move(message),
                                    mandatory,
                                    immediate);
    } catch (std::exception& e) {
        logger.throw_error<std::runtime_error>(LOG_SRC, "Producer failed to publish on exchange "
            "{} with routing key {} with the following error message {}", options.exchange_name, routing_key, e.what());
    }
}