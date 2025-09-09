#pragma once

#include <optional>
#include <flow/Producer.hpp>

namespace pt::flow {
    template<typename In, typename Out>
    class Module : public Producer<Out> {
    public:
        using input_type = In;

        explicit Module(const ProductionPolicy policy = ProductionPolicy::Fanout): Producer<Out>(policy) {
        }

        virtual std::optional<Out> process(In input) = 0;

        std::any process_any(std::any in, size_t producer_id) override {
            auto result = process(std::move(utils::any_cast_with_info<In>(std::move(in))));
            if (result.has_value())
                return std::move(result.value());
            return std::nullopt;
        }
    };
}
