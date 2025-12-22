#pragma once

#include <flow/Producer.hpp>
#include <flow/ProductionPolicy.h>
#include <optional>

namespace pt::flow {
template<typename Out>
class Source : public Producer<Out> {
public:
    using output_type = Out;

    explicit Source(ProductionPolicy policy = ProductionPolicy::Fanout) : Producer<Out>(policy) {}

    /**
     * Function for the user to implement the logic in
     * @return optional typed result
     */
    virtual std::optional<Out> process() = 0;

    /**
     * Non-typed process function
     * @return The non-typed output
     */
    nstd::any process_any(nstd::any, size_t producer_id) override {  // cpp:S1238
        auto result = process();
        if (result.has_value()) {
            return std::move(result.value());
        }
        return std::nullopt;
    }
};
}  // namespace pt::flow