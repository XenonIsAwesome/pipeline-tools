#pragma once

#include <flow/Flow.hpp>
#include <utils/exceptions/bad_any_cast_with_info.hpp>

namespace pt::flow {
template<typename In>
class Sink : public Flow {
public:
    using input_type = In;

    /**
     * Function for the user to implement the logic in
     * @param input The typed input data
     */
    virtual void process(In input) = 0;

    /**
     * Overriding because sink doesn't need to produce to the next flow objects
     */
    void execute(nstd::any in, size_t producer_id) override {
        if (!in.has_value() || in.type() == typeid(std::nullopt_t)) {
            return;
        }
        process_any(std::move(in), producer_id);
    }

    /**
     * Non-typed process function
     * @param in The non-typed input data
     * @param producer_id The producer id (usually 0, only used in aggregator)
     */
    nstd::any process_any(nstd::any in, size_t producer_id) override {
        auto cast_input = utils::any_cast_with_info<In>(std::move(in));
        process(std::move(cast_input));

        return {};
    }
};
}  // namespace pt::flow