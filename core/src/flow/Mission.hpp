#pragma once

#include <atomic>
#include <flow/Pipeline.hpp>
#include <iostream>
#include <modules/io/ConstantSource.hpp>
#include <modules/io/queue/QueueSink.hpp>
#include <modules/io/queue/QueueSource.hpp>
#include <thread>
#include <utils/queues/QueueFactory.hpp>
#include <utils/threads/Worker.h>

namespace pt::flow {
namespace helpers {

/**
 * Gets the input_type typedef of the first flow given in a variadic template
 * This is the default case
 */
template<typename...>
struct get_input_type_from_first_flow;

/**
 * Gets the input_type typedef of the first flow given in a variadic template
 * This the case when 1 type or more are given
 *
 * @tparam First The first flow object
 * @tparam Rest The rest of the flow objects
 */
template<typename First, typename... Rest>
    requires concepts::FlowLike<First> && (... && concepts::FlowLike<Rest>)
struct get_input_type_from_first_flow<First, Rest...> {
    using type = typename First::input_type;
};

/**
 * Gets the input_type typedef of the first flow given in a variadic template
 * This the case when no types are given
 */
template<>
struct get_input_type_from_first_flow<> {
    using type = void;
};

/**
 * Gets the input_type typedef of the first flow given in a variadic template
 * Ease of use typedef, gets the type straight away
 *
 * @tparam First The first flow object
 * @tparam Rest The rest of the flow objects
 */
template<concepts::FlowLike First, concepts::FlowLike... Rest>
using get_input_type_from_first_flow_t =
    typename get_input_type_from_first_flow<First, Rest...>::type;

/**
 * Gets the output_type typedef of the last flow given in a variadic template
 * This the default case
 */
template<typename...>
struct get_output_type_from_last_flow {
    using type = void;
};

/**
 * Gets the output_type typedef of the last flow given in a variadic template
 * This is the case when more than one type is given (recursive template)
 *
 * @tparam T The Last flow object
 */
template<concepts::FlowLike T>
struct get_output_type_from_last_flow<T> {
    using type = typename T::output_type;
};

/**
 * Gets the output_type typedef of the last flow given in a variadic template
 * This is the case when more than one type is given (recursive template)
 *
 * @tparam T The first flow object (removed in recursion)
 * @tparam Rest The rest of the flow objects
 */
template<concepts::FlowLike T, concepts::FlowLike... Rest>
struct get_output_type_from_last_flow<T, Rest...> : get_output_type_from_last_flow<Rest...> {};

/**
 * Gets the output_type typedef of the last flow given in a variadic template
 * Ease of use typedef, gets the type straight away
 *
 * @tparam T The first flow object (removed in recursion)
 * @tparam Rest The rest of the flow objects
 */
template<concepts::FlowLike T, concepts::FlowLike... Rest>
using get_output_type_from_last_flow_t = typename get_output_type_from_last_flow<T, Rest...>::type;

}  // namespace helpers

/**
 * To create a Mission based on this object, you must implement the `build()` method.
 * Use one of, or both of the `add` method varients, to add pipelines to the mission.
 */
class Mission {
public:
    explicit Mission(queues::QueueType queue_type = queues::QueueType::LOCK_FREE) :
        queue_type(queue_type) {}

    virtual ~Mission() = default;

    void start(bool strict = true);

    void stop();

protected:
    /**
     * User override point, this is where you implement the building of the mission
     * With the use of the `pt::flow::Mission::add method` to add pipelines to the missions.
     *
     * Please look into `MockManualMission` and `MockAutoMission` in
     * `core/test/flow/mission_tests.cpp`
     */
    virtual void build() = 0;

    /**
     * Adding a pipeline constructed manually
     * @param pipeline The pipeline object
     */
    void add(Pipeline&& pipeline) {
        pipelines.emplace_back(std::move(pipeline));
    }

    /**
     * A function that adds a full pipeline to the mission
     * @param sources A tuple of sources to add to the start of the pipeline
     * @param nodes A tuple of nodes (modules/aggregators) to add to the pipeline
     * @param sinks A tuple of sinks to add to the end of the pipeline
     */
    template<concepts::SourceLike... Sources,
             concepts::NodeLike... Nodes,
             concepts::SinkLike... Sinks>
    void add(std::tuple<std::shared_ptr<Sources>...> sources,
             std::tuple<std::shared_ptr<Nodes>...> nodes,
             std::tuple<std::shared_ptr<Sinks>...> sinks) {
        Pipeline p;

        if constexpr (sizeof...(Sources) != 0) {
            apply_flows_to_pipeline(p, sources);
        } else {
            p.add(make_queue_source<Nodes..., Sinks...>());
        }

        if constexpr (sizeof...(Nodes) != 0) {
            apply_flows_to_pipeline(p, nodes);
        }

        if constexpr (sizeof...(Sinks) != 0) {
            apply_flows_to_pipeline(p, sinks);
        } else {
            p.add(make_queue_sink<Sources..., Nodes...>());
        }

        pipelines.emplace_back(p);
    }

private:
    /**
     * Adds all the flows in a tuple to a given pipeline object.
     * @tparam Flows Variadic template for the types of the flow objects
     * @param p The pipeline object
     * @param flows A tuple of the flow objects
     */
    template<concepts::FlowLike... Flows>
    void apply_flows_to_pipeline(Pipeline& p, std::tuple<std::shared_ptr<Flows>...> flows) {
        std::apply([&p](auto&&... f) { (p.add(f), ...); }, flows);
    }

    /**
     * Type-erased queue wrapper
     * So you won't have to input all the queue types when creating the mission
     */
    struct QueueWrapper {
        nstd::any queue;
        bool used{false}; /**< Used by the queue auto-connect feature to sign if a queue is in used
                             already or not. */
    };

    /// The queues moving data between pipelines, used for auto-connect feature
    std::vector<QueueWrapper> queues;
    queues::QueueType queue_type; /**< Type of queues to use in the mission */

    /**
     * Returns the needed queue to auto-connect them between pipelines.
     * @returns Either the last queue, or a new one.
     */
    template<typename T>
    std::shared_ptr<queues::IQueue<T>> get_or_create_queue() {
        if (!queues.empty() && !queues.back().used) {
            queues.back().used = true;
            return utils::any_cast_with_info<std::shared_ptr<queues::IQueue<T>>>(
                queues.back().queue);
        }

        auto q = make_queue<T>(queue_type);
        queues.emplace_back(q);

        return q;
    }


    /**
     * Auto-connecting a QueueSource using the input_type of the first flow object.
     * @tparam Flows variadic template of the types of the flow objects
     * @return Typed QueueSource (as Flow, for type erasure)
     */
    template<concepts::FlowLike... Flows>
    auto make_queue_source() {
        using queue_t = typename helpers::get_input_type_from_first_flow<Flows...>::type;

        static_assert(!std::is_same_v<queue_t, void>,
                      "Cannot deduce queue_type: both Nodes and Sinks are empty.");

        auto q = get_or_create_queue<queue_t>();
        return std::make_shared<modules::QueueSource<queue_t>>(q);
    }


    /**
     * Auto-connecting a QueueSink using the input_type of the last flow object.
     * @tparam Flows variadic template of the types of the flow objects
     * @return Typed QueueSink (as Flow, for type erasure)
     */
    template<concepts::FlowLike... Flows>
    auto make_queue_sink() {
        using queue_t = typename helpers::get_output_type_from_last_flow<Flows...>::type;

        static_assert(!std::is_same_v<queue_t, void>,
                      "Cannot deduce queue_type: both Nodes and Sources are empty.");

        auto q = get_or_create_queue<queue_t>();
        return std::make_shared<modules::QueueSink<queue_t>>(q);
    }

    std::vector<Pipeline> pipelines; /**< The pipelines held in the mission */

    std::vector<std::unique_ptr<threads::Worker>> workers; /**< The workers running the pipelines */
    std::atomic_flag stop_flag; /**< atomic flag to stop the workers when requested */
};
}  // namespace pt::flow