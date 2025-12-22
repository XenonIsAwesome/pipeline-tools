#pragma once

#include "Consumer.h"

template<pt::ipc::rmq::concepts::RMQSerialized T>
pt::ipc::rmq::Consumer<T>::Consumer(const RMQOptions& options) :
    MQEntity(options, "Consumer") {
    bool queue_exists;
    try {
        queue_exists = this->channel->CheckQueueExists(options.queue_name);
    } catch (std::exception) {
        queue_exists = false;
    }

    if (!queue_exists) {
        this->channel->DeclareQueue(options.queue_name,
                                    options.queue_passive,
                                    options.queue_durable,
                                    options.queue_exclusive,
                                    options.queue_auto_delete);
    }

    consumer_tag = this->channel->BasicConsume(options.queue_name);
}

template<pt::ipc::rmq::concepts::RMQSerialized T>
pt::ipc::rmq::Consumer<T>::~Consumer() {
    this->stop();
}

template<pt::ipc::rmq::concepts::RMQSerialized T>
std::optional<T> pt::ipc::rmq::Consumer<T>::consume(const std::chrono::milliseconds timeout) {
    AmqpClient::Envelope::ptr_t envelope;

    auto result = this->channel->BasicConsumeMessage(this->consumer_tag, envelope, timeout.count());
    if (!result) {
        return std::nullopt;
    }

    auto consumed_item = envelope->Message()->Body();
    if constexpr (concepts::JSONSerialized<T>) {
        auto json_doc = nlohmann::json::parse(consumed_item);
        return T(json_doc);
    } else if constexpr (concepts::StringSerialized<T>) {
        return T(consumed_item);
    } else if constexpr (std::is_same_v<T, nlohmann::json>) {
        return nlohmann::json::parse(consumed_item);
    } else if constexpr (std::is_same_v<T, std::string>) {
        return consumed_item;
    }
    return std::nullopt;
}

template<pt::ipc::rmq::concepts::RMQSerialized T>
void pt::ipc::rmq::Consumer<T>::stop() {
    if (channel) {
        channel->BasicCancel(consumer_tag);
        channel.reset();
    }
}
