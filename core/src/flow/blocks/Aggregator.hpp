#pragma once

#include <cmath>
#include <flow/Producer.hpp>
#include <flow/ProductionPolicy.h>
#include <optional>
#include <unordered_map>
#include <utils/exceptions/bad_any_cast_with_info.hpp>

namespace pt::flow {
template<typename In, typename Out>
class Aggregator : public Producer<Out> {
public:
    using input_type = In;
    using output_type = Out;

    explicit Aggregator() : Producer<Out>(ProductionPolicy::Fanout) {}

    /**
     * Collects results from producers based on the policy given in the ctor
     * Uses a bitmask to make sure data was received from all producers.
     * @param in The input data
     * @param producer_id The ID of the producer block the data came from
     * @return The output data, cast into nstd::any
     */
    nstd::any process_any(nstd::any in, size_t producer_id) override {
        size_t idx = 1 << producer_id;

        if (state & idx) {
            /// Shouldn't happen, this is a just in case kinda thing.
            return {};
        }

        latest[producer_id] = std::move(utils::any_cast_with_info<In>(std::move(in)));
        state |= idx;

        if (state == static_cast<int>(std::pow(2, producer_count)) - 1) {
            state = 0;

            std::vector<In> collected;
            collected.reserve(producer_count);

            for (auto [_, v] : latest) {
                collected.emplace_back(v);
            }
            latest.clear();

            auto result = process(std::move(collected));
            if (result.has_value()) {
                return std::move(result.value());
            }
            return {};
        }

        return {};
    }

    /**
     * Function for the user to implement the logic in
     * @param inputs The vector of collected outputs from the previous modules
     * @return optional typed result
     */
    virtual std::optional<Out> process(std::vector<In> inputs) = 0;

protected:
    /**
     * Gives an id for the producer so the aggreagator can
     * recognize which producer gave which result.
     * @return ID for the given producer
     */
    size_t register_producer(std::shared_ptr<Flow>) override {
        return producer_count++;
    }

private:
    size_t state{0};
    size_t producer_count{0};
    std::unordered_map<size_t, Out> latest;
};
}  // namespace pt::flow