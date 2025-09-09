#pragma once

#include <utils/queues/MoodycamelQueue.hpp>
#include <utils/queues/moodycamel/readerwriterqueue.h>

namespace pt::utils::queues {
template<typename T>
class BlockingQueue final: public MoodycamelQueue<moodycamel::BlockingReaderWriterQueue<T>, T> {};
}