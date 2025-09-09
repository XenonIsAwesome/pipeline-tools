#pragma once

#include <optional>
#include <flow/Producer.hpp>

namespace pt::flow {
    template<typename Out>
    class Source : public Producer<Out> {
    public:
        explicit Source(const ProductionPolicy policy = ProductionPolicy::Fanout): Producer<Out>(policy) {
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
