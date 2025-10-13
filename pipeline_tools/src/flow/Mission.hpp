#pragma once

#include <ranges>
#include <utils/threads/Worker.h>
#include <modules/io/queue/QueueSink.hpp>
#include <modules/io/queue/QueueSource.hpp>
#include <flow/concepts.h>
#include <flow/Pipeline.hpp>
#include <flow/PipelineProvider.h>
#include <utils/queues/IQueue.hpp>
#include <utils/queues/QueueFactory.hpp>

namespace pt::flow {
    namespace helpers {
        template<typename...>
        struct get_input_type_of_first_flow;

        template<typename First, typename... Rest>
        struct get_input_type_of_first_flow<First, Rest...> {
            using type = typename First::input_type;
        };

        template<>
        struct get_input_type_of_first_flow<> {
            using type = void;
        };

        template<typename ...Flows>
        using get_input_type_of_first_flow_t = typename get_input_type_of_first_flow<Flows...>::type;

        template<typename ...>
        struct get_output_type_of_last_flow {
            using type = void;
        };

        template<typename Last>
        struct get_output_type_of_last_flow<Last> {
            using type = typename Last::output_type;
        };

        template<typename First, typename ...Rest>
        struct get_output_type_of_last_flow<First, Rest...> {
            using type = typename get_output_type_of_last_flow<Rest...>::type;
        };

        template<typename ...Flows>
        using get_output_type_of_last_flow_t = typename get_output_type_of_last_flow<Flows...>::type;
    }

    template<concepts::PipelineProvider InputPP = void, concepts::PipelineProvider OutputPP = void>
    class Mission {
    public:
        explicit Mission(queues::QueueType queue_type = queues::QueueType::LOCK_FREE): queue_type(queue_type) {}
        virtual ~Mission() = default;

        /**
         * Starts all pipelines in the opposite order they were created in.
         * (last pipeline added, starts first)
         */
        void start(){
            // build all the pipelines
            this->build_wrapper();

            // clear old workers if Mission was reused
            workers.clear();
            workers.reserve(pipelines.size());

            // start pipelines in reverse order
            for (auto& pipeline : std::ranges::reverse_view(pipelines)) {
                threads::ThreadPolicy policy{
                    .cores = 1,
                    .affinity_type = threads::AffinityType::PINNED,
                    .priority = THREAD_PRIORITY_LOWEST
                };

                auto func = [&pipeline](std::atomic_flag& flag) -> void {
                    while (!flag.test(std::memory_order_relaxed)) {
                        pipeline.execute();
                    }
                };
                workers.emplace_back(std::make_shared<threads::Worker>(std::move(policy), std::move(func)));
                workers.back()->start();
            }
        }

        /**
         * Stops all pipeline in the order they were created in.
         * (last pipeline added, stops last)
         */
        void stop(){
            for (auto& w : std::ranges::reverse_view(workers)) {
                w->stop();
            }
            workers.clear();
        }

    protected:
        /**
         * User override point
         * The user will inherit from pt::flow::Mission and override this method,
         * and will use the pt::flow::Mission::add method inside it to add pipelines.
         */
        virtual void build() = 0;

        void add(const Pipeline& p) {
            pipelines.push_back(p);
        }

        template<typename ...Sources, typename ...Nodes, typename ...Sinks>
        requires (... && concepts::SourceLike<Sources>) &&
                (... && concepts::NodeLike<Nodes>) &&
                (... && concepts::SinkLike<Sinks>)
        void add(std::tuple<std::shared_ptr<Sources>...> sources,
            std::tuple<std::shared_ptr<Nodes>...> nodes,
            std::tuple<std::shared_ptr<Sinks>...> sinks)
        {
            Pipeline p;

            if constexpr (sizeof...(Sources) != 0) {
                std::apply([&p](auto &&... src) {
                    ((p.add(src)), ...);
                }, sources);
            } else {
                p.add(make_queue_source<Nodes..., Sinks...>());
            }

            if constexpr (sizeof...(Nodes) != 0) {
                std::apply([&p](auto &&... node) {
                    ((p.add(node)), ...);
                }, nodes);
            }

            if constexpr (sizeof...(Sinks) != 0) {
                std::apply([&p](auto &&... sink) {
                    ((p.add(sink)), ...);
                }, sinks);
            } else {
                p.add(make_queue_sink<Sources..., Nodes...>());
            }

            pipelines.push_back(p);
        }

    private:
        void build_wrapper() {
            if constexpr (!std::is_same_v<InputPP, void>) {
                InputPP input_pp;
                add(input_pp.build_input());
            }

            build();

            if constexpr (!std::is_same_v<OutputPP, void>) {
                OutputPP output_pp;
                add(output_pp.build_output());
            }
        }

        queues::QueueType queue_type;

        struct QueueWrapper {
            std::any queue;
            bool used{false};
        };
        std::vector<QueueWrapper> queues;

        template<typename T>
        std::shared_ptr<queues::IQueue<T>> get_or_create_queue() {
            if (!queues.empty() && !queues.back().used) {
                queues.back().used = true;
                return utils::any_cast_with_info<std::shared_ptr<queues::IQueue<T>>>(queues.back().queue);
            }
            auto q = queues::make_queue<T>(queue_type);
            queues.emplace_back(q);
            return q;
        }

        template<typename ...Flows>
        requires (... && concepts::FlowLike<Flows>)
        auto make_queue_source() {
            using queue_t = helpers::get_input_type_of_first_flow_t<Flows...>;
            static_assert(!std::is_same_v<queue_t, void>, "Cannot create a QueueSource, Both nodes and sinks are empty!");

            auto queue = get_or_create_queue<queue_t>();
            return std::make_shared<modules::QueueSource<queue_t>>(queue);
        }

        template<typename ...Flows>
        requires (... && concepts::FlowLike<Flows>)
        auto make_queue_sink() {
            using queue_t = helpers::get_output_type_of_last_flow_t<Flows...>;
            static_assert(!std::is_same_v<queue_t, void>, "Cannot create a QueueSink, Both sources and nodes are empty!");

            auto queue = get_or_create_queue<queue_t>();
            return std::make_shared<modules::QueueSink<queue_t>>(queue);
        }

        std::vector<Pipeline> pipelines; /**< The pipelines held in the mission */
        std::vector<std::shared_ptr<threads::Worker>> workers; /**< threads running each pipeline */
    };
}