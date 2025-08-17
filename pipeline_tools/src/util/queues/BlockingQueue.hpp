#pragma once

#include <util/queues/IQueue.hpp>
#include <util/queues/moodycamel/readerwriterqueue.h>

namespace pt::queues {
    template<typename T>
    class LockFreeQueue final: public IQueue<T> {
        using MoodyQueue = moodycamel::BlockingReaderWriterQueue<T>;

    public:
        explicit LockFreeQueue(): queue_() {}

        T& peek() override {
            return queue_.peek();
        }

        bool pop(T& item) override {
            return queue_.try_dequeue(item);
        }

        bool push(const T& item) override {
            return queue_.try_enqueue(std::move(item));
        }

        bool push(const T&& item) override {
            return queue_.try_enqueue(std::move(item));
        }

    private:
        MoodyQueue queue_;
    };
}
