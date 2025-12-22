#pragma once

#include <flow/blocks/Sink.hpp>
#include <utils/ipc/rmq/concepts.h>
#include <utils/ipc/rmq/Publisher.h>

namespace pt::modules::io::ipc::rmq {

template<pt::ipc::rmq::concepts::RMQSerialized T>
class RMQSink final : public flow::Sink<T> {
public:
    /**
     * @param options RabbitMQ connection options
     * @param routing_key The RabbitMQ routing key to publish to.
     * @param mandatory Requires the message to be delivered to a queue.
     * @param immediate Requires the message to be both routed to a queue and
     * immediately delivered to a consumer
     */
    RMQSink(pt::ipc::rmq::RMQOptions options,
            const std::string& routing_key = "",
            bool mandatory = false,
            bool immediate = false);

    /**
     * Processing function, publishes the data
     * @param input Data to publish.
     */
    void process(T input) override;

private:
    pt::ipc::rmq::Publisher<T> publisher;
    std::string routing_key;
    bool mandatory;
    bool immediate;
};
}  // namespace pt::modules::io::ipc::rmq

#include "RMQSink.tpp"