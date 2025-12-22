#pragma once

#include <utils/queues/BlockingQueue.hpp>
#include <utils/queues/LockFreeQueue.hpp>
#include <utils/queues/StdQueue.hpp>

namespace pt::queues {
enum class QueueType {
    STD_QUEUE, /**< std::queue */
    BLOCKING, /**< moodycamel::BlockingReaderWriterQueue */
    LOCK_FREE /**< moodycamel::ReaderWriterQueue */
};

template<typename T>
std::shared_ptr<IQueue<T>> make_queue(const QueueType queue_type) {
    switch (queue_type) {
        case QueueType::STD_QUEUE:
            return std::make_shared<StdQueue<T>>();
        case QueueType::BLOCKING:
            return std::make_shared<BlockingQueue<T>>();
        case QueueType::LOCK_FREE:
            return std::make_shared<LockFreeQueue<T>>();
    }
    return std::make_shared<StdQueue<T>>();
}
}  // namespace pt::queues