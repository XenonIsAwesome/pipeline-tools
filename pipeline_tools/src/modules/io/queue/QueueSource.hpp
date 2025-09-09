#pragma once

#include <flow/blocks/Source.hpp>
#include <optional>
#include <utils/queues/IQueue.hpp>

namespace pt::modules {
    namespace concepts {
        template<typename T>
        concept ImplicitlyDefaultConstructible = std::__is_implicitly_default_constructible<T>::value;
    }

    template<concepts::ImplicitlyDefaultConstructible T>
    class QueueSource : public flow::Source<T> {
    public:
        explicit QueueSource(std::shared_ptr<utils::queues::IQueue<T>> queue): queue_(std::move(queue)) {
        }

        std::optional<T> process() override {
            T item;
            if (queue_->pop(item)) {
                return item;
            }
            return std::nullopt;
        }

    private:
        std::shared_ptr<utils::queues::IQueue<T>> queue_;
    };
} // namespace pt::modules
