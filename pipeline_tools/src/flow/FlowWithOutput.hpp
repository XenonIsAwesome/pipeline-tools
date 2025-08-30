#pragma once

#include <flow/Flow.hpp>
#include <utils/exceptions/bad_any_cast_with_info.hpp>

namespace pt::flow {
    template<typename Out>
    class FlowWithOutput : public Flow {
    public:
        using output_type = Out;

        FlowWithOutput(ProductionPolicy policy = ProductionPolicy::Fanout): Flow(policy) {
        }

    protected:
        void round_robin(std::any output) override {
            throw std::runtime_error("RoundRobin not supported for this type");
        }
    };

    // specialization for when Out itself is a vector (e.g. Out = std::vector<int>)
    template<typename T>
    class FlowWithOutput<std::vector<T> > : public Flow {
    public:
        using output_type = std::vector<T>;

        FlowWithOutput(ProductionPolicy policy = ProductionPolicy::Fanout): Flow(policy) {
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
