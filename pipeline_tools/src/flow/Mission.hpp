#pragma once

#include <thread>
#include <modules/io/queue/QueueSink.hpp>
#include <modules/io/queue/QueueSource.hpp>
#include <flow/concepts.h>
#include <flow/Pipeline.hpp>
#include <utils/queues/IQueue.hpp>
#include <utils/queues/QueueFactory.hpp>

namespace pt::flow {
    class Mission {
    public:
        explicit Mission(queues::QueueType queue_type): queue_type(queue_type) {}
        virtual ~Mission() = default;

        /**
         * Starts all pipelines in the opposite order they were created in.
         * (last pipeline added, starts first)
         */
        void start();

        /**
         * Stops all pipeline in the order they were created in.
         * (last pipeline added, stops last)
         */
        void stop();

    protected:
        /**
         * User override point
         * The user will inherit from pt::flow::Mission and override this method,
         * and will use the pt::flow::Mission::add method inside it to add pipelines.
         */
        virtual void build() = 0;

        template<typename ...Srcs, typename ...Nods, typename ...Snks>
        requires (... && concepts::SourceLike<Srcs>) &&
                (... && concepts::NodeLike<Nods>) &&
                (... && concepts::SinkLike<Snks>)
        void add(std::tuple<std::shared_ptr<Srcs>...> sources,
            std::optional<std::tuple<std::shared_ptr<Nods>...>> nodes,
            std::tuple<std::shared_ptr<Snks>...> sinks)
        {
            Pipeline p;

            std::apply([&p](auto &&... src) {
                ((p.add(src)), ...);
            }, sources.value());

            if (nodes.has_value()) {
                std::apply([&p](auto &&... node) {
                    ((p.add(node)), ...);
                }, nodes.value());
            }

            std::apply([&p](auto &&... sink) {
                ((p.add(sink)), ...);
            }, sinks.value());

            pipelines.emplace_back(p);
        }

        template<typename ...Nods, typename ...Snks>
        requires (... && concepts::NodeLike<Nods>) &&
                (... && concepts::SinkLike<Snks>)
        void add(std::optional<std::tuple<std::shared_ptr<Nods>...>> nodes,
            std::tuple<std::shared_ptr<Snks>...> sinks)
        {
            if constexpr (sizeof...(Nods) > 0) {
                using first_node = std::tuple_element_t<0, std::tuple<Nods...>>;
                using queue_output_type = typename first_node::input_type;

                auto q = get_or_create_queue<queue_output_type>();
                auto src = std::make_shared<modules::QueueSource<queue_output_type>>(q);

                add(std::make_tuple(src), nodes, sinks);
            } else {
                // TODO: allow for empty pipelines (maybe)
                throw std::runtime_error("Pipeline is empty");
            }
        }

        template<typename ...Srcs, typename ...Nods>
        requires (... && concepts::SourceLike<Srcs>) &&
                (... && concepts::NodeLike<Nods>)
        void add(std::optional<std::tuple<std::shared_ptr<Srcs>...>> sources,
            std::optional<std::tuple<std::shared_ptr<Nods>...>> nodes)
        {
            if constexpr (sizeof...(Nods) > 0) {
                using last_node = typename std::tuple_element_t<sizeof...(Nods) - 1,
                                    std::tuple<std::shared_ptr<Nods>...>>::element_type;
                using queue_input_type = typename last_node::output_type;

                auto q = get_or_create_queue<queue_input_type>();
                auto sink = std::make_shared<modules::QueueSink<queue_input_type>>(q);

                add(sources, nodes, std::make_tuple(sink));
            } else {
                // TODO: allow for empty pipelines (maybe)
                throw std::runtime_error("Pipeline is empty");
            }
        }

        template<typename ...Nods>
        requires (... && concepts::NodeLike<Nods>)
        void add(std::optional<std::tuple<std::shared_ptr<Nods>...>> nodes)
        {
            if constexpr (sizeof...(Nods) > 0) {
                // Input Queue
                using first_node = std::tuple_element_t<0, std::tuple<Nods...>>;
                using queue_output_type = typename first_node::input_type;

                auto input_q = get_or_create_queue<queue_output_type>();
                auto src = std::make_shared<modules::QueueSource<queue_output_type>>(input_q);

                // Output Queue
                using last_node = typename std::tuple_element_t<sizeof...(Nods) - 1,
                        std::tuple<std::shared_ptr<Nods>...>>::element_type;
                using queue_input_type = typename last_node::output_type;

                auto output_q = get_or_create_queue<queue_input_type>();
                auto sink = std::make_shared<modules::QueueSink<queue_input_type>>(output_q);

                add(std::make_tuple(src), nodes, std::make_tuple(sink));
            } else {
                // TODO: allow for empty pipelines (maybe)
                throw std::runtime_error("Pipeline is empty");
            }
        }

    private:
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
                return std::any_cast<std::shared_ptr<queues::IQueue<T>>&>(queues.back().queue);
            }
            auto q = queues::make_queue<T>(queue_type);
            queues.push_back({q, true});
            return q;
        }

        std::vector<Pipeline> pipelines; /**< The pipelines held in the mission */

        /** control flag (false = running, true = stop requested) */
        std::atomic_flag stop_flag = ATOMIC_FLAG_INIT;
        std::vector<std::thread> workers; /**< threads running each pipeline */
    };
}