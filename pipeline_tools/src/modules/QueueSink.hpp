#pragma once

#include <flow/blocks/Sink.hpp>
#include <util/queues/IQueue.hpp>

namespace pt::modules {
    // TODO(xenon): add docs
    template<typename T>
    class QueueSink final : public flow::blocks::Sink<T> {
    public:
        // TODO(xenon): add docs if needed
        QueueSink(std::string name, std::shared_ptr<queues::IQueue<T>> queue):
            flow::blocks::Sink<T>(std::move(name)), queue_(queue) {}

        // TODO(xenon): add docs if needed
        void process(const T& item) override;
    private:
        std::shared_ptr<queues::IQueue<T>> queue_;
    };

    template<typename T>
    void QueueSink<T>::process(const T& item) {
        queue_->push(std::move(item));
        std::cout << "sink pushing item" << std::endl;
    }
} // namespace pt::modules
