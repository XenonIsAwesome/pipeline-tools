#pragma once

#include <utils/queues/MoodycamelQueue.hpp>
#include <utils/queues/moodycamel/readerwriterqueue.h>

namespace pt::queues {
    template<typename T>
    class LockFreeQueue final: public MoodycamelQueue<moodycamel::ReaderWriterQueue<T>, T> {};
}
