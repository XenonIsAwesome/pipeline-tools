#pragma once

template<pt::ipc::rmq::concepts::RMQSerialized T>
void pt::modules::io::ipc::rmq::RMQSink<T>::process(T input) {
    publisher.publish(input, routing_key, mandatory, immediate);
}

template<pt::ipc::rmq::concepts::RMQSerialized T>
pt::modules::io::ipc::rmq::RMQSink<T>::RMQSink(pt::ipc::rmq::RMQOptions options,
        const std::string& routing_key = "",
        bool mandatory = false,
        bool immediate = false) :
    flow::Sink<T>(), publisher(options, routing_key), routing_key(routing_key),
    mandatory(mandatory), immediate(immediate) {}