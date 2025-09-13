#pragma once

#include <utils/threads/Worker.h>
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
                }, sources.value());
            } else {
                p.add(make_queue_source<Nodes..., Sinks...>());
            }

            if constexpr (sizeof...(Nodes) != 0) {
                std::apply([&p](auto &&... node) {
                    ((p.add(node)), ...);
                }, nodes.value());
            }

            if constexpr (sizeof...(Sinks) != 0) {
                std::apply([&p](auto &&... sink) {
                    ((p.add(sink)), ...);
                }, sinks.value());
            } else {
                p.add(make_queue_sink<Sources..., Nodes...>());
            }

            pipelines.push_back(p);
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
                return utils::any_cast_with_info<std::shared_ptr<queues::IQueue<T>>&>(queues.back().queue);
            }
            auto q = queues::make_queue<T>(queue_type);
            queues.emplace_back(q);
            return q;
        }

        template<typename ...Nodes, typename ...Sinks>
        std::shared_ptr<Flow> make_queue_source() {
            using queue_type = std::conditional_t<
                sizeof...(Nodes) != 0,
                typename std::tuple_element_t<0, std::tuple<Nodes...>>::input_type,
                std::conditional_t<
                    sizeof...(Sinks) != 0,
                    typename std::tuple_element_t<0, std::tuple<Sinks...>>::input_type,
                    void
                >
            >;

            static_assert(std::is_same_v<queue_type, void>, "Cannot create a QueueSource, Both nodes and sinks are empty!");

            auto queue = get_or_create_queue<queue_type>();
            return std::make_shared<modules::QueueSource<queue_type>>(queue);
        }

        template<typename ...Sources, typename ...Nodes>
        std::shared_ptr<Flow> make_queue_sink() {
            using queue_type = std::conditional_t<
                sizeof...(Nodes) != 0,
                typename std::tuple_element_t<sizeof...(Nodes) - 1, std::tuple<Nodes...>>::output_type,
                std::conditional_t<
                    sizeof...(Sources) != 0,
                    typename std::tuple_element_t<sizeof...(Sources) - 1, std::tuple<Sources...>>::output_type,
                    void
                >
            >;

            static_assert(std::is_same_v<queue_type, void>, "Cannot create a QueueSink, Both sources and nodes are empty!");

            auto queue = get_or_create_queue<queue_type>();
            return std::make_shared<modules::QueueSink<queue_type>>(queue);
        }

        std::vector<Pipeline> pipelines; /**< The pipelines held in the mission */
        std::vector<threads::Worker> workers; /**< threads running each pipeline */
    };
}