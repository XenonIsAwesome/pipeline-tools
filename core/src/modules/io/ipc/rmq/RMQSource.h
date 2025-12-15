#pragma once

#include <flow/blocks/Source.hpp>
#include <utils/ipc/rmq/Consumer.h>

namespace pt::modules::io::ipc::rmq {

template<pt::ipc::rmq::concepts::RMQSerialized T>
class RMQSource final : public flow::Source<T> {
public:
    /**
     * @param options RabbitMQ connection options
     * @param policy Flow policy to use
     * @param timeout The timeout to consumption
     */
    RMQSource(pt::ipc::rmq::RMQOptions options,
              const flow::ProductionPolicy policy = flow::ProductionPolicy::Fanout,
              std::chrono::milliseconds timeout = pt::ipc::rmq::default_rmq_consume_timeout);

    /**
     * Processing method, returns the consumed item on success.
     * @return The consumed item (if succeeded to consume)
     */
    std::optional<T> process() override;

private:
    pt::ipc::rmq::Consumer<T> consumer;
    std::chrono::milliseconds consume_timeout;
};
}  // namespace pt::modules::io::ipc::rmq

#include "RMQSource.tpp"