#pragma once

#include <queue>
#include <utils/queues/IQueue.hpp>
#include <mutex>
#include <stdexcept>
#include <dawg-log/logger.hpp>

namespace pt::queues {
template<typename T>
class StdQueue : public IQueue<T> {
public:
    StdQueue() = default;
    ~StdQueue() override = default;

    T& peek() override {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.front();
    }

    bool pop(T& item) override {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) {
            return false;
        }

        item = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    bool push(const T& item) override {
        if constexpr (std::is_copy_constructible_v<T>) {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(item);
            return true;
        } else {
            DawgLog::throw_error<std::runtime_error>(LOG_SRC,
                "Cannot push move-only type via const reference");
        }
    }

    bool push(T&& item) override {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(std::move(item));
        return true;
    }

private:
    std::queue<T> queue_ {};
    std::mutex mtx_;
};

}  // namespace pt::queues