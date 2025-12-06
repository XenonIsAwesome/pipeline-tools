#pragma once

#include <flow/blocks/Source.hpp>
#include <utils/ipc/rmq/Consumer.hpp>

namespace pt::modules {
template<typename T>
class RMQSource: flow::Source<T> {
public:
    RMQSource(const ipc::rmq::RMQOptions &options, std::chrono::milliseconds timeout = std::chrono::milliseconds(100));

    std::optional<T> process();

private:
    ipc::rmq::Consumer<T> consumer;
    std::chrono::milliseconds timeout;
};
}