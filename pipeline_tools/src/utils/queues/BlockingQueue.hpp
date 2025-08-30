#pragma once

#include <utils/queues/Queue.hpp>
#include <utils/queues/moodycamel/readerwriterqueue.h>

namespace pt::utils::queues {
    template<typename T>
    class LockFreeQueue final: public Queue<T> {
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
