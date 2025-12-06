#pragma once
#include "MQEntity.h"
#include "concepts.h"

#include <string>

namespace pt::ipc::rmq {
template<concepts::RMQSerializer T>
class Publisher: public MQEntity {
public:
    explicit Publisher(const RMQOptions& options, std::string routing_key = ""): MQEntity(options) {
        if (routing_key.empty()) {
            routing_key = options.queue_name;
        }
        this->routing_key = routing_key;

        if (!channel->CheckExchangeExists(options.exchange_name)) {
            channel->DeclareExchange(
                options.exchange_name,
                options.exchange_type,
                options.exchange_passive,
                options.exchange_durable,
                options.exchange_auto_delete);
        }

        if (!channel->CheckQueueExists(options.queue_name)) {
            channel->DeclareQueue(
                options.queue_name,
                options.queue_passive,
                options.queue_durable,
                options.queue_exclusive,
                options.queue_auto_delete);
        }
    }

    void publish(T message, bool mandatory = false, bool immediate = false) {
        auto msg = AmqpClient::BasicMessage::Create();

        if constexpr (concepts::JSONSerializer<T>) {
            msg->ContentType("application/json");

            auto json_doc = message.serialize();
            msg->Body(json_doc.dump());
        } else if constexpr (concepts::StringSerializer<T>) {
            msg->Body(message.serialize());
        } else if constexpr (std::is_same_v<T, nlohmann::json>) {
            msg->ContentType("application/json");
            msg->Body(message.dump());
        } else if constexpr (std::is_same_v<T, std::string>) {
            msg->Body(message);
        }

        channel->BasicPublish(options.exchange_name, routing_key, msg, mandatory, immediate);
    }

private:
    std::string routing_key;
};
}