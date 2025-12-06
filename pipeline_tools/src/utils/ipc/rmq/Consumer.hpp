#pragma once
#include "MQEntity.h"
#include "concepts.h"

#include <string>

namespace pt::ipc::rmq {
template<concepts::RMQDeserializer T>
class Consumer: public MQEntity {
public:
    explicit Consumer(const RMQOptions &options): MQEntity(options) {
        if (routing_key.empty()) {
            routing_key = options.queue_name;
        }
        this->routing_key = routing_key;

        if (!channel->CheckQueueExists(options.queue_name)) {
            channel->DeclareQueue(
                options.queue_name,
                options.queue_passive,
                options.queue_durable,
                options.queue_exclusive,
                options.queue_auto_delete);
        }

        consumer_tag = channel->BasicConsume(options.queue_name, "");
    }

    std::optional<T> consume(std::chrono::milliseconds timeout = std::chrono::milliseconds(100)) {
        AmqpClient::Envelope::ptr_t envelope;

        channel->BasicConsumeMessage(consumer_tag, envelope, timeout.count());
        if (envelope == nullptr) {
            return std::nullopt;
        }

        auto body = envelope->Message()->Body();
        if constexpr (concepts::StringDeserializer<T>) {
            return T(body);
        } else if constexpr (concepts::JSONDeserializer<T>) {
            auto json_doc = nlohmann::json::parse(body);
            return T(json_doc);
        }

        return std::nullopt;
    }

private:
    std::string consumer_tag;
    std::string routing_key;
};
}