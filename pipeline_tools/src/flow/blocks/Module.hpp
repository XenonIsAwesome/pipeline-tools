#pragma once

#include <optional>
#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename In, typename Out>
    class Module : public Flow {
    public:
        using input_type = In;
        using output_type = Out;

        explicit Module(std::string name, const ProductionPolicy policy):
            Flow(std::move(name), policy) {}

        virtual std::optional<Out> process(const In& input) = 0;

        std::any process_any(const std::any& in, size_t producer_id) override {
            auto result = process(std::any_cast<In>(in));
            if (result.has_value())
                return std::any(result.value());
            return std::nullopt;
        }
    };
}
