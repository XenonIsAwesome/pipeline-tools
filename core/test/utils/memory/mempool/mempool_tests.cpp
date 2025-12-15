#include <future>
#include <gtest/gtest.h>
#include <utils/memory/mempool/MemPool.hpp>

TEST(MemPoolTest, ConstructWithValidArgs) {
    pt::memory::MemPool<int> pool(1024, 4);
    EXPECT_EQ(pool.capacity(), 4u);
    EXPECT_EQ(pool.block_size(), 1024u);
}

TEST(MemPoolTest, AllocateAndDeallocate) {
    pt::memory::MemPool<char> pool(64, 2);

    auto buf1 = pool.allocate();
    auto buf2 = pool.allocate();

    // Should throw when out of blocks
    EXPECT_THROW(pool.allocate(), std::runtime_error);

    // Deallocating one should allow another allocation
    buf1.reset();  // or let it go out scope to release
    auto buf3 = pool.allocate();
    EXPECT_NE(buf3.get(), nullptr);
}

TEST(MemPoolTest, AvailableCount) {
    pt::memory::MemPool<int> pool(64, 2);
    EXPECT_EQ(pool.available(), 2u);

    auto b1 = pool.allocate();
    EXPECT_EQ(pool.available(), 1u);

    auto b2 = pool.allocate();
    EXPECT_EQ(pool.available(), 0u);

    // Should now be zero
    EXPECT_THROW(pool.allocate(), std::runtime_error);
}

TEST(MemPoolTest, ConstructorInvalidArgs) {
    EXPECT_THROW((pt::memory::MemPool<int>(0, 1)), std::invalid_argument);
    EXPECT_THROW((pt::memory::MemPool<int>(1, 0)), std::invalid_argument);
}

// TEST(MemPoolTest, UniqueBufferCustomDeleter) {
//     using namespace pt::memory;
//
//     MemPool<char> pool(64, 2);
//
//     auto buf = pool.allocate();
//     char* ptr = buf.get();
//
//     EXPECT_NE(ptr, nullptr);
//     buf.reset(); // Triggers custom deleter
//
//     // Now we should be able to reallocate that same block
//     auto new_buf = pool.allocate();
//     EXPECT_EQ(new_buf.get(), ptr);  // Same pointer! // TODO: FIX: test fails
// }

/// NOTE: ⚠️ If using std::thread, include <future> and link with -pthread.
TEST(MemPoolTest, ThreadSafeAllocation) {
    pt::memory::MemPool<char> pool(64, 10);
    std::vector<std::future<void>> futures;

    for (int i = 0; i < 100; ++i) {
        futures.push_back(std::async(std::launch::async, [&pool]() {
            auto buf = pool.allocate();
            // Do something small
        }));
    }

    for (auto& f : futures)
        f.wait();
}