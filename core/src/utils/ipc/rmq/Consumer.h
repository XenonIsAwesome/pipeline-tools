#pragma once

#include "concepts.h"
#include "MQEntity.h"

#include <chrono>
#include <string>

namespace pt::ipc::rmq {
static constexpr std::chrono::milliseconds default_rmq_consume_timeout =
    std::chrono::milliseconds(100);

/**
 * A RabbitMQ Consumer class, made to consume a value from a given RabbitMQ queue when requested
 * @tparam T The type of the consume item.
 */
template<concepts::RMQSerialized T>
class Consumer : public MQEntity {
public:
    /**
     * @param options RabbitMQ connection options
     */
    explicit Consumer(const RMQOptions& options);

    ~Consumer();

    /**
     * Consumes a value from the RabbitMQ queue
     * @param timeout The amount of time to wait for a consume
     * @return The value consumed from the RabbitMQ queue, or nullopt if the timeout was over.
     */
    std::optional<T> consume(std::chrono::milliseconds timeout = default_rmq_consume_timeout);

    /**
     * Stops the connection to the RabbitMQ queue.
     */
    void stop();

private:
    std::string consumer_tag;
    RMQOptions options;
};
}  // namespace pt::ipc::rmq

#include "Consumer.tpp"
