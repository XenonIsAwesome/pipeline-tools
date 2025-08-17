#pragma once

#include <queue>
#include <util/queues/IQueue.hpp>

namespace pt::queues {
    template<typename T>
    class Queue final: public IQueue<T> {
    public:
        explicit Queue(): queue_() {}

        T& peek() override {
            return queue_.front();
        }

        bool pop(T& item) override {
            if (queue_.empty()) return false;

            item = this->peek();
            queue_.pop();

            return true;
        }

        bool push(const T& item) override {
            queue_.push(std::move(item));
            return true;
        }

        bool push(const T&& item) override {
            queue_.push(std::move(item));
            return true;
        }

    private:
        std::queue<T> queue_;
    };
}