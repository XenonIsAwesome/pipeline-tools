#pragma once

#include <any>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>


namespace pt::flow {
    enum class ProductionPolicy {
        RoundRobin, /**< Give each output to each consumer: <br>`consumers[i]->consume(outputs[i])` */
        Fanout /**< Copy output to all consumers but move to the last */
    };

    class Flow {
    public:
        virtual ~Flow() = default;

        using input_type = void;
        using output_type = void;

        explicit Flow(const ProductionPolicy policy = ProductionPolicy::Fanout): policy(policy) {
        }

        virtual std::any process_any(std::any in, size_t producer_id) = 0;

        void add_next(const std::shared_ptr<Flow> &next) {
            size_t id = next->register_producer(next);
            next_nodes.emplace_back(next, id);
        }

        void execute(std::any in = {}, size_t producer_id = 0) {
            auto output = process_any(std::move(in), producer_id);
            if (output.type() != typeid(std::nullopt_t) && output.has_value())
                produce(std::move(output));
        }

    protected:
        virtual void produce(std::any output) {
            if (policy == ProductionPolicy::RoundRobin) [[likely]] {
                round_robin(std::move(output));
            } else if (policy == ProductionPolicy::Fanout) [[likely]] {
                for (size_t i = 0; i < next_nodes.size() - 1; ++i) {
                    auto &[next, producer_id] = next_nodes[i];
                    next->execute(output, producer_id);
                }
                auto &[last, producer_id] = next_nodes.at(next_nodes.size() - 1);
                last->execute(std::move(output), producer_id);
            } else {
                std::cerr << "Unknown production policy" << std::endl;
            }
        }

        /**
         * Consumer override point (e.g. Aggregator)
         * @return The ID of the producer
         */
        virtual size_t register_producer(std::shared_ptr<Flow>) { return 0; }

        /**
         * Consumer override point (e.g. Producer)
         * @param output The output to produce in a round-robin way
         */
        virtual void round_robin(std::any output) {
            throw std::runtime_error("Flow objects with a RoundRobin policy must return a vector");
        }

        // (next node, producer_id for *that* consumer)
        std::vector<std::pair<std::shared_ptr<Flow>, size_t> > next_nodes;

    private:
        ProductionPolicy policy;
    };

    /**
     * Manual connect (for branching/fanout/etc.)
     * @param from The flow object to wire from
     * @param to The flow object to wire to
     */
    static void connect(const std::shared_ptr<Flow> &from,
                        const std::shared_ptr<Flow> &to) {
        from->add_next(to);
    }
} // namespace pt::flow
