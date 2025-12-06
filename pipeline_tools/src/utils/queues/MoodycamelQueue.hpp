#pragma once

#include <stdexcept>
#include <type_traits>
#include <utils/queues/IQueue.hpp>
#include <utils/queues/moodycamel/readerwriterqueue.h>

namespace pt::queues {
namespace concepts {
template<typename Q, typename T>
concept MoodyQueue = requires(Q q, T t) {
    { q.try_enqueue(t) } -> std::same_as<bool>;
    { q.try_dequeue(t) } -> std::same_as<bool>;
    { q.peek() } -> std::same_as<T*>;
};
}   // namespace concepts

template<typename T, concepts::MoodyQueue<T> Q>
class MoodycamelQueue : public IQueue<T> {
  public:
    explicit MoodycamelQueue() : queue_() {}

    T& peek() override {
        return *queue_.peek();
    }

    bool pop(T& item) override {
        return queue_.try_dequeue(item);
    }

    bool push(const T& item) override {
        if constexpr (std::is_copy_constructible_v<T>) {
            return queue_.try_enqueue(item);
        } else {
            throw std::runtime_error(
                "Cannot push move-only type via const reference");
        }
    }
    bool push(T&& item) override {
        return queue_.try_enqueue(std::move(item));
    }

  private:
    Q queue_;
};
}   // namespace pt::queues
