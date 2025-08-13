#pragma once

#include <flow/blocks/Sink.hpp>

#include <optional>
#include <iostream>

template<typename T>
concept Printable = requires(T t) {
    std::cout << t;
};

namespace pt::modules {
    // TODO(xenon): add docs
    template<Printable T>
    class PrintSink: public flow::blocks::Sink<T> {
    public:
        // TODO(xenon): add docs if needed
        PrintSink(): flow::blocks::Sink<T>("PrintSink") {}
        
        // TODO(xenon): add docs if needed
        void process(const T& input) override;
    };
} // namespace pt::modules

template<Printable T>
void pt::modules::PrintSink<T>::process(const T & input) {
    std::cout << input << std::endl;
}
