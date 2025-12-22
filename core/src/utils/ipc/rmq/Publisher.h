#pragma once

#include "MQEntity.h"
#include <SimpleAmqpClient/Channel.h>
#include <string>

namespace pt::ipc::rmq {
/**
 * A RabbitMQ Publisher class, made to publish a value from to a given RabbitMQ queue when requested
 * @tparam T
 */
template<concepts::RMQSerialized T>
class Publisher : public MQEntity {
public:
    /**
     * @param options RabbitMQ connection options
     * @param routing_key The RabbitMQ routing key to publish to.
     * @throws std::runtime_error when failing to create a channel.
     */
    explicit Publisher(const RMQOptions& options, const std::string& routing_key = "");

    /**
     * Publishes a value to the RabbitMQ queue.
     * @param value The value to publish.
     * @param mandatory Requires the message to be delivered to a queue.
     * @param immediate Requires the message to be both routed to a queue and
     * immediately delivered to a consumer
     */
    void publish(T value, bool mandatory = false, bool immediate = false);

private:
    RMQOptions options;
    std::string routing_key;
};
}  // namespace pt::ipc::rmq


#include "Publisher.tpp"
