#pragma once
#include "BlockingQueue.hpp"
#include "IQueue.hpp"
#include "LockFreeQueue.hpp"
#include "StdQueue.hpp"


#include <memory>

namespace pt::queues {
    enum class QueueType {
        STD_QUEUE, /**< A queue implementation based on `std::queue` */
        BLOCKING, /**< A queue implementation based on `moodycamel::BlockingReaderWriterQueue` */
        LOCK_FREE /**< A queue implementation based on `moodycamel::ReaderWriterQueue` */
    };

    /**
     * Makes a typed-queue based on the queue type given.
     * @tparam T Type of the item in the queue
     * @param type The type of queue to make
     * @return `std::shared_ptr` of the selected queue type
     */
    template<typename T>
    std::shared_ptr<IQueue<T>> make_queue(QueueType type) {
        switch (type) {
            case QueueType::STD_QUEUE:
                return std::make_shared<StdQueue<T>>();
            case QueueType::BLOCKING:
                return std::make_shared<BlockingQueue<T>>();
            case QueueType::LOCK_FREE:
                return std::make_shared<LockFreeQueue<T>>();
            default:
                throw std::runtime_error("Unknown QueueType");
        }
    }
}