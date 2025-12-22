#pragma once

#include <flow/Producer.hpp>
#include <flow/ProductionPolicy.h>
#include <optional>
#include <utils/exceptions/bad_any_cast_with_info.hpp>

namespace pt::flow {
template<typename In, typename Out>
class Module : public Producer<Out> {
public:
    using input_type = In;
    using output_type = Out;

    explicit Module(const ProductionPolicy policy = ProductionPolicy::Fanout) :
        Producer<Out>(policy) {}

    /**
     * Function for the user to implement the logic in
     * @param input The typed input data
     * @return optional typed result
     */
    virtual std::optional<Out> process(In input) = 0;

    /**
     * Non-typed process function
     * @param in The non-typed input data
     * @param producer_id The producer id (usually 0, only used in aggregator)
     * @return The non-typed output
     */
    nstd::any process_any(nstd::any in, size_t producer_id) override {
        auto cast_input = utils::any_cast_with_info<In>(std::move(in));
        auto result = std::move(process(std::move(cast_input)));

        if (result.has_value()) {
            return std::move(result.value());
        }
        return std::nullopt;
    }
};
}  // namespace pt::flow