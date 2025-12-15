#pragma once

#include <flow/blocks/Sink.hpp>
#include <utils/queues/IQueue.hpp>

namespace pt::modules {
template<typename T>
class QueueSink : public flow::Sink<T> {
public:
    explicit QueueSink(std::shared_ptr<queues::IQueue<T>> queue) : flow::Sink<T>(), queue(queue) {}

    void process(T input) override {
        queue->push(std::move(input));
    }

private:
    std::shared_ptr<queues::IQueue<T>> queue;
};
}  // namespace pt::modules