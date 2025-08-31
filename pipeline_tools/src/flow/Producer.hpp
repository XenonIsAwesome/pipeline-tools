#pragma once

#include <flow/Flow.hpp>
#include <utils/exceptions/bad_any_cast_with_info.hpp>

namespace pt::flow {
    /**
     * Generic typed extension of Flow for modules producing single outputs.
     *
     * This template is used when a module's output is a single value of type `Out`
     * (e.g., `int`, `std::string`, custom structs, ...). It provides a typed
     * interface and allows Fanout/Broadcast policies to be implemented without
     * requiring the output to be wrapped in a vector.
     *
     * @note For RoundRobin policy, modules must instead produce a vector of values.
     *       That case is handled by the partial specialization
     *       `FlowWithOutput<std::vector<T>>`.
     *
     * @tparam Out The type produced by the module.
     */
    template<typename Out>
    class Producer : public Flow {
    public:
        using output_type = Out;

        Producer(ProductionPolicy policy = ProductionPolicy::Fanout): Flow(policy) {
        }

    protected:
        void round_robin(std::any output) override {
            Flow::round_robin(output);
        }
    };

    /**
     * Partial specialization for when Out itself is a vector (e.g. Out = std::vector<int>)
     *
     * This specialization handles the case where the module already produces
     * a vector of values (`std::vector<T>`). Instead of wrapping a vector
     * inside another vector, the RoundRobin dispatcher distributes each
     * element of the produced vector across different downstream consumers.
     *
     * For example, if the module produces `std::vector<int>` with values
     * `{1, 2, 3}` and three downstream consumers are connected, then
     * consumer 0 receives `1`, consumer 1 receives `2`, and consumer 2
     * receives `3`.
     *
     * This allows modules to directly produce batch outputs, while still
     * supporting per-consumer distribution under the RoundRobin policy.
     *
     * @tparam T The element type contained in the output vector.
     */
    template<typename T>
    class Producer<std::vector<T> > : public Flow {
    public:
        using output_type = std::vector<T>;

        Producer(ProductionPolicy policy = ProductionPolicy::Fanout): Flow(policy) {
        }

    protected:
        void round_robin(std::any output) override {
            auto outputs = utils::any_cast_with_info<std::vector<T> >(std::move(output));
            size_t max_size = std::min(outputs.size(), next_nodes.size());

            for (size_t i = 0; i < max_size; i++) {
                auto &[next, id] = next_nodes[i];
                next->execute(std::move(outputs.at(i)), id);
            }
        }
    };
}
