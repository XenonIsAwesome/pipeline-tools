#pragma once

#include <optional>
#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename In, typename Out>
    class Module : public Flow {
    public:
        using input_type = In;
        using output_type = Out;

        explicit Module(const ProductionPolicy policy = ProductionPolicy::Fanout): Flow(policy) {}

        virtual std::optional<Out> process(In input) = 0;

        std::any process_any(std::any in, size_t producer_id) override {
            auto result = process(std::any_cast<In>(std::move(in)));
            if (result.has_value())
                return std::move(result.value());
            return std::nullopt;
        }
    };
}
