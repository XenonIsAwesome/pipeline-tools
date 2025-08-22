#pragma once

#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename In, typename Out>
    class Module : public Flow {
    public:
        explicit Module(std::string name, const ProductionPolicy policy):
            Flow(std::move(name), policy) {}

        virtual Out process(const In& input) = 0;

        std::any process_any(const std::any& in) override {
            return process(std::any_cast<In>(in));
        }
    };
}
