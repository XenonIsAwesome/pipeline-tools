#pragma once

#include <flow/Flow.hpp>

namespace pt::flow {
    template<typename Out>
    class Source : public Flow {
    public:
        using output_type = Out;

        explicit Source(std::string name, const ProductionPolicy policy):
            Flow(std::move(name), policy) {}

        virtual std::optional<Out> process() = 0;

        std::any process_any(const std::any&, size_t producer_id) override {
            auto result = process();
            if (result.has_value())
                return std::any(result.value());
            return {};
        }
    };
}
