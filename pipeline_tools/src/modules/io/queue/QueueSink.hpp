#pragma once

#include <flow/blocks/Sink.hpp>
#include <utils/queues/IQueue.hpp>

namespace pt::modules {
    template<typename T>
    class QueueSink final : public flow::Sink<T> {
    public:
        explicit QueueSink(std::shared_ptr<queues::IQueue<T>> queue): queue_(queue) {
        }

        void process(const T &item) override {
            queue_->push(std::move(item));
        }

    private:
        std::shared_ptr<queues::IQueue<T>> queue_;
    };
} // namespace pt::modules
