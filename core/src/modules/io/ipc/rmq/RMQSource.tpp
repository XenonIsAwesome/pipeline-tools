#pragma once

template<pt::ipc::rmq::concepts::RMQSerialized T>
std::optional<T> pt::modules::io::ipc::rmq::RMQSource<T>::process() {
    return consumer.consume(consume_timeout);
}

template<pt::ipc::rmq::concepts::RMQSerialized T>
pt::modules::io::ipc::rmq::RMQSource<T>::RMQSource(pt::ipc::rmq::RMQOptions options,
          const flow::ProductionPolicy policy, std::chrono::milliseconds timeout) :
    flow::Source<T>(policy), consumer(options), consume_timeout(timeout) {}