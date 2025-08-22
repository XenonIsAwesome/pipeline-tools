#pragma once

#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename In>
    class Sink : public Flow {
    public:
        explicit Sink(std::string name) : Flow(std::move(name), ProductionPolicy::NoConsumer) {}

        virtual void process(const In& input) = 0;

        std::any process_any(const std::any& in, size_t producer_id) override {
            process(std::any_cast<In>(in));
            return {};
        }
    };
}
