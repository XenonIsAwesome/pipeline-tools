#pragma once
#include <flow/blocks/Aggregator.hpp>
#include <flow/blocks/Module.hpp>
#include <flow/blocks/Sink.hpp>
#include <flow/blocks/Source.hpp>
#include <flow/exceptions/unknown_flow_object.h>
#include <flow/exceptions/unordered_sources.h>
#include <flow/Flow.hpp>
#include <memory>

namespace pt::flow {

namespace concepts {
template<typename F>
concept FlowLike = std::derived_from<F, Flow>;

template<typename F>
concept SourceLike = std::derived_from<F, Source<typename F::output_type>>;

template<typename F>
concept ModuleLike = std::derived_from<F, Module<typename F::input_type, typename F::output_type>>;

template<typename F>
concept AggregatorLike =
    std::derived_from<F, Aggregator<typename F::input_type, typename F::output_type>>;

template<typename F>
concept NodeLike = ModuleLike<F> || AggregatorLike<F>;

template<typename F>
concept SinkLike = std::derived_from<F, Sink<typename F::input_type>>;

}  // namespace concepts

class Pipeline {
public:
    Pipeline() = default;

    /**
     * Add a flow object to the pipeline
     *
     * Pipeline::add will auto-wire all sources to the first module / aggregator.<br/>
     * Pipeline::add will auto-wire all modules and aggregators linearly.<br/>
     * Pipeline::add will auto-wire the last module / aggregator to all sinks.
     *
     * \image html PipelineBehaviour.png
     *
     * Usage:
     *   1. Add ALL sources.
     *   2. Add ALL modules / aggregators.
     *   3. Add ALL sinks.
     *
     * @param f pt::flow::Flow object to add to the pipeline
     * @return The same flow object that was added to the pipeline
     * @throws exceptions::unordered_sources When a source is added after a
     * module/aggregator/sink.
     * @throws exceptions::unknown_flow_object When an unknown object derived from Flow is added.
     */
    template<typename F>
    std::shared_ptr<F> add(std::shared_ptr<F> f)
        requires concepts::FlowLike<F>
    {
        if constexpr (concepts::SourceLike<F>) {
            sources.push_back(f);
            if (!nodes.empty()) {
                throw exceptions::unordered_sources();
            }
        } else if constexpr (concepts::NodeLike<F> || concepts::SinkLike<F>) {
            if (!nodes.empty()) {
                nodes.back()->add_consumer(f);
            } else if (!sources.empty()) {
                for (auto& src : sources) {
                    src->add_consumer(f);
                }
            }

            if constexpr (!concepts::SinkLike<F>) {
                nodes.push_back(f);
            }
        } else {
            throw exceptions::unknown_flow_object(typeid(F));
        }

        return f;
    }

    /**
     * Trigger execution (start all sinks, or first module)
     */
    void execute() const {
        if (!sources.empty()) {
            for (auto& src : sources) {
                src->execute();
            }
        } else if (!nodes.empty()) {
            nodes.front()->execute();
        }
    }

private:
    std::vector<std::shared_ptr<Flow>> sources;
    std::vector<std::shared_ptr<Flow>> nodes;
};
}  // namespace pt::flow