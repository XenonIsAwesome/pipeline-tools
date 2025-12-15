#pragma once

#include <flow/blocks/Source.hpp>
#include <utils/queues/IQueue.hpp>

namespace pt::concepts {
template<typename T>
concept ImplicitlyDefaultConstructible = std::__is_implicitly_default_constructible<T>::value;
} // namespace pt::concepts

namespace pt::modules {
template<concepts::ImplicitlyDefaultConstructible T>
class QueueSource : public flow::Source<T> {
public:
    explicit QueueSource(std::shared_ptr<queues::IQueue<T>> queue) : flow::Source<T>(), queue(queue) {}

    std::optional<T> process() override {
        T out;
        if (!queue->pop(out)) {
            return std::nullopt;
        }
        return std::move(out);
    }

private:
    std::shared_ptr<queues::IQueue<T>> queue;
};
}  // namespace pt::modules