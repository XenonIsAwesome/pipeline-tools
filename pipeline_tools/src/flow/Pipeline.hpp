#pragma once

#include "concepts.h"


#include <memory>
#include <vector>
#include <flow/Flow.hpp>
#include <flow/blocks/Source.hpp>
#include <flow/blocks/Sink.hpp>
#include <flow/blocks/Aggregator.hpp>
#include <flow/blocks/Module.hpp>
#include <utils/exceptions/unknown_flow_object_type.h>

namespace pt::flow {
    class Pipeline {
    public:
        /**
         * Add a flow object to the pipeline.
         *
         * Pipeline::add will auto-wire all sources to the first module / aggregator.<br/>
         * Pipeline::add will auto-wire modules & aggregators linearly.<br/>
         * Pipeline::add will auto-wire the last module / aggregator to all sinks.
         *
         * \image html PipelineBehaviour.png
         *
         * Usage:
         *   1. Add ALL sources
         *   2. Add ALL modules/aggregators (if there are any)
         *   3. Add ALL sinks
         *
         * Not using the function correctly will cause runtime errors / segfaults.
         *
         * @tparam F pt::flow::Flow object (type)
         * @tparam FIn Input type of the flow object
         * @tparam FOut Output type of the flow object
         * @param f pt::flow::Flow object to add to the pipeline
         * @return The same flow object that was added to the pipeline
         * @throws utils::exceptions::unknown_flow_object_type When an unknown flow object
         */
        template<
            typename F,
            typename FIn = typename F::input_type,
            typename FOut = typename F::output_type>
        std::shared_ptr<F> add(std::shared_ptr<F> f)
            requires std::derived_from<F, Flow> {
            if constexpr (std::derived_from<F, Source<FOut> >) {
                sources.push_back(f);
                if (!nodes.empty()) {
                    throw std::runtime_error("Add all the sources FIRST!");
                }
            } else if constexpr (std::derived_from<F, Module<FIn, FOut> > || std::derived_from<F, Aggregator<FIn,
                                     FOut> > || std::derived_from<F, Sink<FIn> >) {
                if (!nodes.empty()) {
                    nodes.back()->add_next(f);
                } else if (!sources.empty()) {
                    for (auto &src: sources) {
                        src->add_next(f);
                    }
                }

                // Only add module and aggregator to the modules
                if constexpr (!std::derived_from<F, Sink<FIn> >) {
                    nodes.push_back(f);
                }
            } else {
                throw utils::exceptions::unknown_flow_object_type(typeid(f));
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
        std::vector<std::shared_ptr<Flow> > sources{};
        std::vector<std::shared_ptr<Flow> > nodes{};
    };
} // namespace pt::flow

namespace pt::flow {
    /**
     * Builds a pipeline in one function call
     * @tparam Srcs pt::flow::Source (type)
     * @tparam Nodes pt::flow::Module / pt::flow::Aggregator (type)
     * @tparam Snks pt::flow::Sink (type)
     * @param sources Tuple of pt::flow::Source flow objects
     * @param nodes Tuple of pt::flow::Module / pt::flow::Aggregator flow objects
     * @param sinks Tuple of pt::flow::Sink flow objects
     * @return The fully built pipeline.
     */
    template<
        typename... Srcs,
        typename... Nodes,
        typename... Snks
    >
    Pipeline make_pipeline(
        std::tuple<std::shared_ptr<Srcs>...> sources,
        std::tuple<std::shared_ptr<Nodes>...> nodes,
        std::tuple<std::shared_ptr<Snks>...> sinks
    )
        requires (... && concepts::SourceLike<Srcs>) &&
                 (... && concepts::NodeLike<Nodes>) &&
                 (... && concepts::SinkLike<Snks>) {
        Pipeline p;

        if constexpr (sizeof...(Nodes) == 0) {
            // TODO: allow for empty pipelines (maybe)
            throw std::runtime_error("Pipeline is empty");
        }

        std::apply([&p](auto &&... src) {
            ((p.add(src)), ...);
        }, sources);

        std::apply([&p](auto &&... node) {
            ((p.add(node)), ...);
        }, nodes);

        std::apply([&p](auto &&... snk) {
            ((p.add(snk)), ...);
        }, sinks);

        return p;
    }
} // namespace pt::flow
