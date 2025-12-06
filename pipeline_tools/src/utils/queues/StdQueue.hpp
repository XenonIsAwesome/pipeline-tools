#pragma once

#include <queue>
#include <stdexcept>
#include <type_traits>
#include <utils/queues/IQueue.hpp>

namespace pt::queues {
template<typename T>
class StdQueue final : public IQueue<T> {
  public:
    explicit StdQueue() : queue_() {}

    T& peek() override {
        mutex_.lock();
        auto& val = queue_.front();
        mutex_.unlock();

        return val;
    }

    bool pop(T& item) override {
        mutex_.lock();
        if (queue_.empty()) {
            mutex_.unlock();
            return false;
        }

        item = std::move(queue_.front());
        queue_.pop();
        mutex_.unlock();

        return true;
    }

    bool push(const T& item) override {
        if constexpr (std::is_copy_constructible_v<T>) {
            mutex_.lock();
            queue_.push(item);
            mutex_.unlock();
            return true;
        } else {
            throw std::runtime_error(
                "Cannot push move-only type via const reference");
        }
    }

    bool push(T&& item) override {
        mutex_.lock();
        queue_.push(std::move(item));
        mutex_.unlock();
        return true;
    }

  private:
    std::queue<T> queue_;
    std::mutex mutex_;
};
}   // namespace pt::queues