#pragma once
#include "concepts.h"
#include "Pipeline.hpp"
#include "utils/queues/IQueue.hpp"
#include "utils/queues/QueueType.h"

namespace pt::flow {
    class Mission {
    public:
        Mission() = default;

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
        requires (... && concepts::SourceLike<Srcs>) && \
                (... && concepts::NodeLike<Nods>) && \
                (... && concepts::SinkLike<Snks>)
        void add(std::optional<std::tuple<std::shared_ptr<Srcs>...>> sources,
            std::optional<std::tuple<std::shared_ptr<Nods>...>> nodes,
            std::optional<std::tuple<std::shared_ptr<Snks>...>> sinks)
        {
            Pipeline p;

            if (sources.has_value()) {
                std::apply([&p](auto &&... src) {
                    ((p.add(src)), ...);
                }, sources.value());
            }

            if (nodes.has_value()) {
                std::apply([&p](auto &&... node) {
                    ((p.add(node)), ...);
                }, nodes.value());
            }

            if (sinks.has_value()) {
                std::apply([&p](auto &&... sink) {
                    ((p.add(sink)), ...);
                }, sinks.value());
            }
        }

        template<typename ...Nods, typename ...Snks>
        requires (... && concepts::NodeLike<Nods>) && \
                (... && concepts::SinkLike<Snks>)
        void add(std::optional<std::tuple<std::shared_ptr<Nods>...>> nodes,
            std::optional<std::tuple<std::shared_ptr<Snks>...>> sinks)
        {
            // TODO: instead of nullopt, make a QueueSource.
            // TODO: for the queue you're supposed to give the QueueSource, do the following:
            // TODO: store all the queues in a vector as a private variable.
            // TODO: track the creation of queues, if a queue was created before this, use the same one to link pipelines to each-other.
            // TODO: if the last queue in the vector was already used to bridge 2 pipelines, then make a new one.
            // TODO: make the fitting pt::queue object based on a given pt::queue::QueueType
            add(std::nullopt, nodes, sinks);
        }

        template<typename ...Srcs, typename ...Nods>
        requires (... && concepts::SourceLike<Srcs>) && \
                (... && concepts::NodeLike<Nods>)
        void add(std::optional<std::tuple<std::shared_ptr<Srcs>...>> sources,
            std::optional<std::tuple<std::shared_ptr<Nods>...>> nodes)
        {
            // TODO: instead of nullopt, make a QueueSink.
            // TODO: for the queue you're supposed to give the QueueSink, do the following:
            // TODO: store all the queues in a vector as a private variable.
            // TODO: track the creation of queues, if a queue was created before this, use the same one to link pipelines to each-other.
            // TODO: if the last queue in the vector was already used to bridge 2 pipelines, then make a new one.
            // TODO: make the fitting pt::queue object based on a given pt::queue::QueueType
            add(sources, nodes, std::nullopt);
        }

        template<typename ...Nods>
        requires (... && concepts::NodeLike<Nods>)
        void add(std::optional<std::tuple<std::shared_ptr<Nods>...>> nodes)
        {
            // TODO: instead of nullopt, make a QueueSource and a QueueSink.
            // TODO: for the queues you're supposed to give the Queue based flow objects, do the following:
            // TODO: store all the queues in a vector as a private variable.
            // TODO: track the creation of queues, if a queue was created before this, use the same one to link pipelines to each-other.
            // TODO: if the last queue in the vector was already used to bridge 2 pipelines, then make a new one.
            // TODO: make the fitting pt::queue object based on a given pt::queue::QueueType
            add(std::nullopt, nodes, std::nullopt);
        }

    private:
        struct QueueWrapper {
            std::any queue;
            bool used{false};
        };
        std::vector<QueueWrapper> queues;

        template<typename T>
        std::shared_ptr<utils::queues::IQueue<T>> get_or_create_queue() {
            if (!queues.empty() && !queues.back().used) {
                queues.back().used = true;
                return std::any_cast<std::shared_ptr<utils::queues::IQueue<T>>&>(queues.back().queue);
            }
            auto q = utils::queues::make_queue<T>(queue_type);
            queues.push_back({q, true});
            return q;
        }

        std::vector<Pipeline> pipelines;

        // TODO: currently not possible to do because we do not know the type of the queue
        // std::vector<pt::queues::IQueue<T>>
    };
}