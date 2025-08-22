#pragma once

#include <any>
#include <iostream>
#include <memory>
#include <vector>
#include <flow/Block.hpp>


namespace pt::flow {
    enum class ProductionPolicy {
        NoConsumer,
        SingleConsumer,
        ManyOutputsManyConsumers,
        SingleOutputManyConsumers
    };

    class Flow : public Block {
    public:
        explicit Flow(std::string name, const ProductionPolicy policy = ProductionPolicy::SingleConsumer):
            Block(std::move(name)), policy(policy) {}

        ~Flow() override = default;

        virtual std::any process_any(const std::any &in) = 0;

        void connect(std::shared_ptr<Flow> next) {
            next_nodes.push_back(std::move(next));
        }

        /**
         * SingleConsumer::produce -> next_nodes.at(0)->execute(output);
         * ManyOutputsManyConsumers::produce -> fanout to consumers: next_nodes.at(i)->execute(outputs.at(i)
         * SingleOutputManyConsumers::produce -> copy to next_nodes.size() - 1 consumers, std::move to the last
         */
        void produce(const std::any& output) {
            if (policy == ProductionPolicy::NoConsumer) {
                return;
            }

            if (policy == ProductionPolicy::SingleConsumer) {
                next_nodes.front()->execute(std::move(output));
            } else if (policy == ProductionPolicy::ManyOutputsManyConsumers) {
                auto outputs = std::any_cast<std::vector<std::any>>(output);
                size_t max_size = std::min(outputs.size(), next_nodes.size());

                for (size_t i = 0; i < max_size; i++) {
                    next_nodes.at(i)->execute(std::move(outputs.at(i)));
                }
            } else if (policy == ProductionPolicy::SingleOutputManyConsumers) {
                for (size_t i = 0; i < next_nodes.size() - 1; ++i) {
                    next_nodes.at(i)->execute(output);
                }
                next_nodes.at(next_nodes.size() - 1)->execute(std::move(output));
            } else {
                std::cerr << "Unknown production policy" << std::endl;
            }
        }

        void execute(const std::any &in = {}) {
            produce(process_any(in));
        }

    private:
        ProductionPolicy policy;
        std::vector<std::shared_ptr<Flow>> next_nodes;
    };
} // namespace pt::flow
