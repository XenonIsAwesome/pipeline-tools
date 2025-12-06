#pragma once

#include <flow/Sink.hpp>
#include <utils/ipc/rmq/Publisher.hpp>

namespace pt::modules {
template<typename T>
class RMQSink: flow::Sink<T> {
public:
    RMQSink(const ipc::rmq::RMQOptions& options, std::string routing_key = "",
        bool mandatory = false, bool immediate = false);

    void process(T input);

private:
    ipc::rmq::Publisher<T> publisher;
    std::string routing_key;
    bool mandatory;
    bool immediate;
};
}