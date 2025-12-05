#pragma once

#include <utils/queues/IQueue.hpp>
#include <utils/queues/moodycamel/readerwriterqueue.h>

namespace pt::queues {
    namespace concepts {
        template<typename Q, typename T>
        concept MoodyQueue = requires(Q q, T t)
        {
            { q.try_enqueue(t) } -> std::same_as<bool>;
            { q.try_dequeue(t) } -> std::same_as<bool>;
            { q.peek() } -> std::same_as<T*>;
        };
    }

    template<typename T, concepts::MoodyQueue<T> Q>
    class MoodycamelQueue : public IQueue<T> {
    public:
        explicit MoodycamelQueue(): queue_() {}

        T& peek() override {
            return *queue_.peek();
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
        Q queue_;
    };
}
