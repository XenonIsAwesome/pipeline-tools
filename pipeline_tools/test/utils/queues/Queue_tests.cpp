#include <atomic>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <thread>
#include <utils/queues/QueueFactory.hpp>
#include <vector>

using namespace pt::queues;

// Helper to map QueueType to a readable name for GTest
template<QueueType T>
struct QueueTypeTrait;

template<>
struct QueueTypeTrait<QueueType::STD_QUEUE> {
    static std::string name() {
        return "StdQueue";
    }
    static constexpr QueueType value = QueueType::STD_QUEUE;
};
template<>
struct QueueTypeTrait<QueueType::BLOCKING> {
    static std::string name() {
        return "BlockingQueue";
    }
    static constexpr QueueType value = QueueType::BLOCKING;
};
template<>
struct QueueTypeTrait<QueueType::LOCK_FREE> {
    static std::string name() {
        return "LockFreeQueue";
    }
    static constexpr QueueType value = QueueType::LOCK_FREE;
};

template<typename T>
class QueueTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // We can't create the queue here because we need the item type, which
        // varies per test
    }
};

// We want to test each QueueType with multiple ItemTypes.
// GTest's TYPED_TEST allows testing one template class with list of types.
// But we have two dimensions: QueueType and ItemType.
// We can fix ItemType for a suite, or fix QueueType.
// Let's try to test all combinations.

// To simplify, let's define a wrapper that holds both.
template<typename TItem, QueueType TQueueType>
struct TypeDefinition {
    using ItemType = TItem;
    static constexpr QueueType QueueTypeValue = TQueueType;
};

// Define the list of types to test
using MyTypes = ::testing::Types<
    TypeDefinition<int, QueueType::STD_QUEUE>,
    TypeDefinition<int, QueueType::BLOCKING>,
    TypeDefinition<int, QueueType::LOCK_FREE>,
    TypeDefinition<std::string, QueueType::STD_QUEUE>,
    TypeDefinition<std::string, QueueType::BLOCKING>,
    TypeDefinition<std::string, QueueType::LOCK_FREE>,
    TypeDefinition<std::vector<int>, QueueType::STD_QUEUE>,
    TypeDefinition<std::vector<int>, QueueType::BLOCKING>,
    TypeDefinition<std::vector<int>, QueueType::LOCK_FREE>,
    TypeDefinition<std::unique_ptr<int>, QueueType::STD_QUEUE>,
    TypeDefinition<std::unique_ptr<int>, QueueType::BLOCKING>,
    TypeDefinition<std::unique_ptr<int>, QueueType::LOCK_FREE>,
    TypeDefinition<std::shared_ptr<int>, QueueType::STD_QUEUE>,
    TypeDefinition<std::shared_ptr<int>, QueueType::BLOCKING>,
    TypeDefinition<std::shared_ptr<int>, QueueType::LOCK_FREE>>;

TYPED_TEST_SUITE(QueueTest, MyTypes);

TYPED_TEST(QueueTest, PushPop) {
    using ItemType = typename TypeParam::ItemType;
    auto queue = make_queue<ItemType>(TypeParam::QueueTypeValue);

    ItemType item;
    if constexpr (std::is_same_v<ItemType, int>)
        item = 42;
    else if constexpr (std::is_same_v<ItemType, std::string>)
        item = "test";
    else if constexpr (std::is_same_v<ItemType, std::vector<int>>)
        item = {1, 2, 3};
    else if constexpr (std::is_same_v<ItemType, std::unique_ptr<int>>)
        item = std::make_unique<int>(100);
    else if constexpr (std::is_same_v<ItemType, std::shared_ptr<int>>)
        item = std::make_shared<int>(200);

    // For unique_ptr, we must move. For others, we can copy or move.
    // make_queue returns shared_ptr<IQueue<T>>

    // Test push(T&&)
    EXPECT_TRUE(queue->push(std::move(item)));

    ItemType popped_item;
    EXPECT_TRUE(queue->pop(popped_item));

    if constexpr (std::is_same_v<ItemType, int>)
        EXPECT_EQ(popped_item, 42);
    else if constexpr (std::is_same_v<ItemType, std::string>)
        EXPECT_EQ(popped_item, "test");
    else if constexpr (std::is_same_v<ItemType, std::vector<int>>)
        EXPECT_EQ(popped_item.size(), 3);
    else if constexpr (std::is_same_v<ItemType, std::unique_ptr<int>>)
        EXPECT_EQ(*popped_item, 100);
    else if constexpr (std::is_same_v<ItemType, std::shared_ptr<int>>)
        EXPECT_EQ(*popped_item, 200);
}

TYPED_TEST(QueueTest, PushConstRef) {
    using ItemType = typename TypeParam::ItemType;

    // Skip for move-only types
    if constexpr (!std::is_copy_constructible_v<ItemType>) {
        return;
    }

    auto queue = make_queue<ItemType>(TypeParam::QueueTypeValue);

    ItemType item;
    if constexpr (std::is_same_v<ItemType, int>)
        item = 42;
    else if constexpr (std::is_same_v<ItemType, std::string>)
        item = "test";
    else if constexpr (std::is_same_v<ItemType, std::vector<int>>)
        item = {1, 2, 3};
    else if constexpr (std::is_same_v<ItemType, std::shared_ptr<int>>)
        item = std::make_shared<int>(200);

    EXPECT_TRUE(queue->push(item));   // Copy push

    ItemType popped_item;
    EXPECT_TRUE(queue->pop(popped_item));

    // Verify value
    if constexpr (std::is_same_v<ItemType, int>)
        EXPECT_EQ(popped_item, 42);
}

TYPED_TEST(QueueTest, Empty) {
    using ItemType = typename TypeParam::ItemType;
    auto queue = make_queue<ItemType>(TypeParam::QueueTypeValue);

    ItemType item;
    EXPECT_FALSE(queue->pop(item));
}

TYPED_TEST(QueueTest, Peek) {
    using ItemType = typename TypeParam::ItemType;
    // Peek might not be supported or safe for all queue types/implementations in
    // a thread-safe way, but IQueue defines it. Note:
    // LockFreeQueue/MoodycamelQueue peek might be tricky if empty, but let's test
    // basic usage.

    auto queue = make_queue<ItemType>(TypeParam::QueueTypeValue);

    ItemType item;
    if constexpr (std::is_same_v<ItemType, int>)
        item = 42;
    else if constexpr (std::is_same_v<ItemType, std::string>)
        item = "test";
    else if constexpr (std::is_same_v<ItemType, std::vector<int>>)
        item = {1, 2, 3};
    else if constexpr (std::is_same_v<ItemType, std::unique_ptr<int>>)
        item = std::make_unique<int>(100);
    else if constexpr (std::is_same_v<ItemType, std::shared_ptr<int>>)
        item = std::make_shared<int>(200);

    queue->push(std::move(item));

    // Peek returns T&.
    // CAUTION: Peek on empty queue is undefined or crashes in some impls if not
    // checked? The implementations seem to assume not empty or handle it?
    // StdQueue::peek locks and returns front. If empty, UB on
    // std::queue::front(). So we must push first.

    auto& peeked = queue->peek();

    if constexpr (std::is_same_v<ItemType, int>)
        EXPECT_EQ(peeked, 42);
    else if constexpr (std::is_same_v<ItemType, std::string>)
        EXPECT_EQ(peeked, "test");
    else if constexpr (std::is_same_v<ItemType, std::unique_ptr<int>>)
        EXPECT_EQ(*peeked, 100);

    ItemType popped;
    queue->pop(popped);
}

TYPED_TEST(QueueTest, Concurrency) {
    using ItemType = typename TypeParam::ItemType;
    // Skip unique_ptr for concurrency test simplicity (copying to threads etc)
    if constexpr (std::is_same_v<ItemType, std::unique_ptr<int>>)
        return;

    auto queue = make_queue<ItemType>(TypeParam::QueueTypeValue);
    const int num_items = 1000;
    std::atomic<int> count{0};

    std::thread producer([&]() {
        for (int i = 0; i < num_items; ++i) {
            ItemType item;
            if constexpr (std::is_same_v<ItemType, int>)
                item = i;
            else if constexpr (std::is_same_v<ItemType, std::string>)
                item = std::to_string(i);
            else if constexpr (std::is_same_v<ItemType, std::vector<int>>)
                item = {i};
            else if constexpr (std::is_same_v<ItemType, std::shared_ptr<int>>)
                item = std::make_shared<int>(i);

            while (!queue->push(std::move(item))) {
                std::this_thread::yield();
            }
        }
    });

    std::thread consumer([&]() {
        for (int i = 0; i < num_items; ++i) {
            ItemType item;
            while (!queue->pop(item)) {
                std::this_thread::yield();
            }
            count++;
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(count, num_items);
}
