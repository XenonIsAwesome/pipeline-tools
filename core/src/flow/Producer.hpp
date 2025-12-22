#pragma once

#include <flow/Flow.hpp>
#include <flow/ProductionPolicy.h>
#include <utils/exceptions/bad_any_cast_with_info.hpp>

namespace pt::flow {

/**
 * Generic typed extension of pt::flow::Flow for modules producing single outputs.
 *
 * This template is used when a module's output is a single value of type `Out`
 * (e.g., `int`, `std::string`, custom structs, ...). It provides a typed
 * interface and allows production policies to be implemented without
 * requiring the output to be wrapped in a vector.
 *
 * @note For ProductionPolicy::RoundRobin, modules must instead produce a vector of values.
 *       That case is handled by the partial specialization `Producer<std::vector<T>>`.
 *
 * @tparam Out The type produced by the producer.
 */
template<typename Out>
class Producer : public Flow {
public:
    using output_type = Out;

    explicit Producer(ProductionPolicy policy = ProductionPolicy::Fanout) : Flow(policy) {}

protected:
    void round_robin_produce(nstd::any output) override {
        Flow::round_robin_produce(std::move(output));
    }
};

/**
 * Partial specialization for when Out itself is a vector. (e.g. Out = std::vector<int>)
 *
 * This specialization handles the case where the module already produces
 * a vector of values (`std::vector<Out>`). Instead of wrapping a vector
 * inside another vector, the producer distributres each element of the
 * produced vector across different consumers.
 *
 * For example, if the producer produces `std::vector<int>{1, 2, 3}`, and three
 * consumers are connected, then consumer 0 receives `1`, consumer 1 receives `2`,
 * and consumer 2 receives `3`.
 *
 * This allows modules to directly produce batch outputs, while still
 * supporting per-consumer distribution under the ProductionPolicy::RoundRobin policy.
 *
 * @tparam Out The element type contained in the output vector.
 */
template<typename Out>
class Producer<std::vector<Out>> : public Flow {
public:
    using output_type = std::vector<Out>;

    explicit Producer(ProductionPolicy policy = ProductionPolicy::Fanout) : Flow(policy) {}

protected:
    void round_robin_produce(nstd::any output) override {
        auto outputs = utils::any_cast_with_info<std::vector<Out>>(std::move(output));
        size_t max_size = std::min(outputs.size(), next_nodes.size());

        for (size_t i = 0; i < max_size; i++) {
            auto& [next, id] = next_nodes[i];
            next->execute(std::move(outputs.at(i)), id);
        }
    }
};
}  // namespace pt::flow