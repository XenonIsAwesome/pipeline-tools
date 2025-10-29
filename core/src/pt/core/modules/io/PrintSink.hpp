#pragma once

#include <iostream>
#include <flow/blocks/Sink.hpp>

namespace pt::modules::concepts {
    template<typename T>
    concept Printable = requires(T t)
    {
        std::cout << t;
    };
}

namespace pt::modules {
    template<concepts::Printable In>
    class PrintSink final: public flow::Sink<In> {
    public:
        void process(In input) override {
            std::cout << input << std::endl;
        }
    };
}