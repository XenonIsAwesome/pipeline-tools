#pragma once

#include <utils/queues/MoodycamelQueue.hpp>
#include <utils/queues/moodycamel/readerwriterqueue.h>

namespace pt::queues {
template<typename T>
class LockFreeQueue : public MoodycamelQueue<T, moodycamel::ReaderWriterQueue<T>> {};

}  // namespace pt::queues