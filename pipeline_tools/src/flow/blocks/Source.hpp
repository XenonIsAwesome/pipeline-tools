#pragma once

#include <flow/Flow.hpp>
#include <optional>

namespace pt::flow {
    template<typename Out>
    class Source : public Flow {
    public:
        using output_type = Out;

        explicit Source(const ProductionPolicy policy = ProductionPolicy::Fanout): Flow(policy) {
        }

        virtual std::optional<Out> process() = 0;

        std::any process_any(std::any, size_t producer_id) override {
            auto result = process();
            if (result.has_value())
                return std::any(result.value());
            return std::nullopt;
        }
    };
}
