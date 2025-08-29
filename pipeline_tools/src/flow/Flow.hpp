#pragma once

#include <any>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>


namespace pt::flow {
    enum class ProductionPolicy {
        NoConsumer, /*< Special case for Sinks */
        RoundRobin, /*< next_nodes.at(i)->execute(outputs.at(i)) */
        Fanout /*< copy to next_nodes.size() - 1 consumers, std::move to the last */
    };

    class Flow {
    public:
        virtual ~Flow() = default;

        using input_type = void;
        using output_type = void;

        explicit Flow(const ProductionPolicy policy = ProductionPolicy::Fanout): policy(policy) {
        }

        virtual std::any process_any(std::any in, size_t producer_id) = 0;

        void connect(const std::shared_ptr<Flow> &next) {
            size_t id = next->register_producer(next);
            next_nodes.emplace_back(next, id);
        }

        void produce(std::any output) {
            if (policy == ProductionPolicy::NoConsumer) return;

            if (policy == ProductionPolicy::RoundRobin) {
                auto outputs = std::any_cast<std::vector<std::any>>(std::move(output));
                size_t max_size = std::min(outputs.size(), next_nodes.size());

                for (size_t i = 0; i < max_size; i++) {
                    auto &[next, id] = next_nodes[i];
                    next->execute(std::move(outputs.at(i)), id);
                }
            } else if (policy == ProductionPolicy::Fanout) {
                for (size_t i = 0; i < next_nodes.size() - 1; ++i) {
                    auto &[next, id] = next_nodes[i];
                    next->execute(output, id);
                }
                auto &[last, id] = next_nodes.at(next_nodes.size() - 1);
                last->execute(std::move(output), id);
            } else {
                std::cerr << "Unknown production policy" << std::endl;
            }
        }

        void execute(std::any in = {}, size_t producer_id = 0) {
            auto output = process_any(std::move(in), producer_id);
            if (output.has_value())
                produce(std::move(output));
        }

    protected:
        // Consumer override point (e.g. Aggregator)
        virtual size_t register_producer(std::shared_ptr<Flow>) { return 0; }

    private:
        ProductionPolicy policy;
        // (next node, producer_id for *that* consumer)
        std::vector<std::pair<std::shared_ptr<Flow>, size_t> > next_nodes;
    };
} // namespace pt::flow
