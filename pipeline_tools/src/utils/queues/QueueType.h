#pragma once

namespace pt::queues {
    enum class QueueType {
        STD_QUEUE, /**< TODO: add docs */
        BLOCKING, /**< TODO: add docs */
        LOCK_FREE /**< TODO: add docs */
    };

    template<typename T>
    std::shared_ptr<utils::queues::IQueue<T>> make_queue(QueueType type) {
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