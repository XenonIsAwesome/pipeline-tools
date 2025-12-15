#pragma once

#include <dawg-log/logger.hpp>
#include <utils/queues/IQueue.hpp>

template<typename Q, typename T>
concept MoodyQueue = requires(Q q, const T t_in, T& t_out) {
    { q.peek() } -> std::same_as<T*>;
    { q.try_dequeue(t_out) } -> std::same_as<bool>;
    { q.try_enqueue(t_in) } -> std::same_as<bool>;
};

namespace pt::queues {
template<typename T, MoodyQueue<T> MoodyQ>
class MoodycamelQueue : public IQueue<T> {
public:
    MoodycamelQueue() = default;
    ~MoodycamelQueue() override = default;

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
            DawgLog::throw_error<std::runtime_error>(LOG_SRC,
                "Cannot push move-only type via const reference");
        }
    }

    bool push(T&& item) override {
        return queue_.try_enqueue(std::move(item));
    }

private:
    MoodyQ queue_ {};
};

}  // namespace pt::queues