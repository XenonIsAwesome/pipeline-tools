#pragma once

#include <memory>
#include <vector>
#include <flow/Flow.hpp>
#include <flow/blocks/Source.hpp>
#include <flow/blocks/Sink.hpp>

namespace pt::flow {
    class Pipeline {
    public:
        /**
         * Add a flow object to the pipeline.
         *
         * Pipeline::add will auto-wire many source to the first module.
         * Pipeline::add will auto-wire the last module to many sinks.
         *
         *      Source3 ---\             --> Sink3
         *                  v           /
         *      Source1 --> Module --> ... --> Sink1
         *                  ^           \
         *      Source2 ---/             --> Sink2
         *
         * @tparam F Flow object (type)
         * @tparam FIn Input type of the flow object
         * @tparam FOut Output type of the flow object
         * @param f Flow object to add to the pipeline
         * @return The same flow object that was added to the pipeline
         *
         * TODO: add auto-connect from all sources to first node.
         */
        template<
            typename F,
            typename FIn = typename F::input_type,
            typename FOut = typename F::output_type
        >
        std::shared_ptr<F> add(std::shared_ptr<F> f) {
            bool add_node = true;
            bool connect_to_last_node = !nodes.empty();

            if constexpr (std::derived_from<F, Source<FOut> >) {
                add_node = sources.empty();
                connect_to_last_node = false;
                sources.push_back(f);
            } else if constexpr (std::derived_from<F, Sink<FIn> >) {
                add_node = false;
            }

            if (connect_to_last_node) {
                nodes.back()->connect(f); // auto-wire previous -> current
            }

            if (add_node) {
                nodes.push_back(f);
            }
            return f;
        }

        /**
         * Trigger execution (start from first node / sources)
         */
        void execute() const {
            if (!sources.empty()) {
                for (auto &src: sources) {
                    src->execute();
                }
            } else if (!nodes.empty()) {
                nodes.front()->execute();
            }
        }

    private:
        bool sink_inserted{false};
        std::vector<std::shared_ptr<Flow> > sources{};
        std::vector<std::shared_ptr<Flow> > nodes{};
    };
} // namespace pt::flow
