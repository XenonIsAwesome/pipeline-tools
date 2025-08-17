#pragma once

#include <flow/blocks/Source.hpp>
#include <optional>
#include <util/queues/IQueue.hpp>
#include <concepts.h>


namespace pt::modules {
    // TODO(xenon): add docs
    template<ImplicitlyDefaultConstructible T>
    class QueueSource : public flow::blocks::Source<T> {
    public:
        // TODO(xenon): add docs if needed
        QueueSource(std::string name, std::shared_ptr<queues::IQueue<T>> queue):
            flow::blocks::Source<T>("QueueSource"), queue_(std::move(queue)) {}

        // TODO(xenon): add docs if needed
        std::optional<T> process() override;
    private:
        std::shared_ptr<queues::IQueue<T>> queue_;
    };

    template<ImplicitlyDefaultConstructible T>
    std::optional<T> QueueSource<T>::process() {
        T item;
        if (queue_->pop(item)) {
            std::cout << "queue popped " << std::endl;
            return item;
        }

        std::cout << "queue failed " << std::endl;
        return std::nullopt;
    }

} // namespace pt::modules
