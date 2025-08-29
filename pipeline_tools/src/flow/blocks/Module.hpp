#pragma once

#include <optional>
#include <flow/FlowWithOutput.hpp>

namespace pt::flow {
    template<typename In, typename Out>
    class Module: public FlowWithOutput<Out> {
    public:
        using input_type = In;

        explicit Module(const ProductionPolicy policy = ProductionPolicy::Fanout): FlowWithOutput<Out>(policy) {}

        virtual std::optional<Out> process(In input) = 0;

        std::any process_any(std::any in, size_t producer_id) override {
            auto result = process(std::any_cast<In>(std::move(in)));
            if (result.has_value())
                return std::move(result.value());
            return std::nullopt;
        }
    };
}
