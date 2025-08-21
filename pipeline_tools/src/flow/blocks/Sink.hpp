#pragma once

#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename In>
    class Sink : public Flow {
    public:
        explicit Sink(std::string name) : Flow(std::move(name)) {}

        virtual void process(const In& input) = 0;

        std::vector<std::any> process_any(const std::any& in) override {
            if (in.has_value()) {
                const In& typed_in = std::any_cast<const In&>(in);
                process(typed_in);
            }
            return {}; // sinks produce no outputs
        }
    };
}
