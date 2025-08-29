#pragma once

#include <optional>
#include <flow/FlowWithOutput.hpp>

namespace pt::flow {
    template<typename Out>
    class Source: public FlowWithOutput<Out> {
    public:
        explicit Source(const ProductionPolicy policy = ProductionPolicy::Fanout): FlowWithOutput<Out>(policy) {
        }

        virtual std::optional<Out> process() = 0;

        std::any process_any(std::any, size_t producer_id) override {
            auto result = process();
            if (result.has_value())
                return std::move(result.value());
            return std::nullopt;
        }
    };
}
