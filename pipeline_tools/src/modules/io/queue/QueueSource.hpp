#pragma once

#include <flow/blocks/Source.hpp>
#include <optional>
#include <utils/queues/IQueue.hpp>
#include <concepts.h>

namespace pt::modules {
    template<typename T>
        requires std::__is_implicitly_default_constructible<T>::value
    class QueueSource : public flow::Source<T> {
    public:
        explicit QueueSource(std::shared_ptr<utils::queues::IQueue> queue): queue_(std::move(queue)) {
        }

        std::optional<T> process() override {
            T item;
            if (queue_->pop_any(item)) {
                return item;
            }
            return std::nullopt;
        }

    private:
        std::shared_ptr<utils::queues::IQueue> queue_;
    };
} // namespace pt::modules
