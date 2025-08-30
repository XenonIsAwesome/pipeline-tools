#pragma once

#include <flow/blocks/Sink.hpp>
#include <utils/queues/IQueue.hpp>

namespace pt::modules {
    template<typename T>
    class QueueSink final : public flow::Sink<T> {
    public:
        explicit QueueSink(std::shared_ptr<utils::queues::IQueue> queue): queue_(queue) {
        }

        void process(const T &item) override {
            queue_->push_any(std::move(item));
        }

    private:
        std::shared_ptr<utils::queues::IQueue> queue_;
    };
} // namespace pt::modules
