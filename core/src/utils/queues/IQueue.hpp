#pragma once

namespace pt::queues {
template<typename T>
/**
 * A generic queue interface to be used in pt (e.g. between threads)
 */
class IQueue {
public:
    virtual ~IQueue() = default;

    /**
     * Same as the `std::queue` implementation
     * @return Copy of the first item in the queue
     */
    [[maybe_unused]] virtual const T& peek() = 0;

    /**
     * Returns and removes the first item in the queue,
     * This differs from the `std::queue` implementation as it does both actions in the same
     * function
     * @return A reference to the first item in the queue
     */
    virtual bool pop(T&) = 0;

    /**
     * Differs from `std::queue` implementation by returning the success of the push as a boolean,
     * can be useful when implementing a queue with limited storage
     * @return True if the push worked, false if it didn't
     */
    virtual bool push(const T&) = 0;

    /**
     * Same as `IQueue::push(const T&)` but with move semantics.
     * @return True if the push worked, false if it didn't
     */
    virtual bool push(T&&) = 0;
};
}  // namespace pt::queues