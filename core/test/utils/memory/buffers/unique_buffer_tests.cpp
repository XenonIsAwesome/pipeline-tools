#include "../MockDeleter.hpp"

#include <gtest/gtest.h>
#include <utils/memory/smart_buffers/unique_buffer.hpp>

class UniqueBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        MockDeleter::deleted = false;
    }
    void TearDown() override {}
};

TEST_F(UniqueBufferTest, DefaultConstruction) {
    pt::memory::unique_buffer<int> buf;
    EXPECT_FALSE(buf);
    EXPECT_EQ(buf.get(), nullptr);
    EXPECT_EQ(buf.size(), 0);
}

TEST_F(UniqueBufferTest, SelfAllocatingConstruction) {
    pt::memory::unique_buffer<int> buf(1024);
    EXPECT_TRUE(buf);
    EXPECT_EQ(buf.size(), 1024);
}

TEST_F(UniqueBufferTest, ConstructionWithDeleter) {
    MockDeleter mock_deleter;

    {
        auto* ptr = new int[10];
        pt::memory::unique_buffer<int> buf(ptr, 10, std::move(mock_deleter));

        EXPECT_TRUE(buf);
        EXPECT_EQ(buf.get(), ptr);
        EXPECT_EQ(buf.size(), 10);
    }

    EXPECT_TRUE(MockDeleter::deleted);
}

TEST_F(UniqueBufferTest, MoveConstruction) {
    MockDeleter mock_deleter;

    auto* ptr = new int[5];
    pt::memory::unique_buffer<int> source(ptr, 5, std::move(mock_deleter));

    // Save the pointer before move
    auto* original_ptr = source.get();

    // Move construct
    pt::memory::unique_buffer moved(std::move(source));

    EXPECT_FALSE(source);  // Should be empty after move
    EXPECT_EQ(source.get(), nullptr);
    EXPECT_TRUE(moved);  // Should own the resource
    EXPECT_EQ(moved.get(), original_ptr);

    // Original should not delete on destruction (moved)
    source.reset();  // This should be a no-op
    EXPECT_FALSE(MockDeleter::deleted);  // Deletion shouldn't happen here
}


TEST_F(UniqueBufferTest, Release) {
    MockDeleter mock_deleter;

    auto* ptr = new int[3];
    pt::memory::unique_buffer<int> buf(ptr, 3, std::move(mock_deleter));

    // Release the buffer
    pt::memory::released_buffer<int> released = buf.release();

    // Should be empty now
    EXPECT_FALSE(buf);
    EXPECT_EQ(buf.get(), nullptr);

    released.deleter(ptr);
    EXPECT_TRUE(MockDeleter::deleted);
}

TEST_F(UniqueBufferTest, Reset) {
    MockDeleter mock_deleter;

    auto* ptr = new int[2];
    pt::memory::unique_buffer<int> buf(ptr, 2, std::move(mock_deleter));

    EXPECT_TRUE(buf);
    EXPECT_FALSE(MockDeleter::deleted);

    // Reset should call deleter
    buf.reset();

    EXPECT_FALSE(buf);  // Should be empty now
    EXPECT_EQ(buf.get(), nullptr);
    EXPECT_TRUE(MockDeleter::deleted);  // Deleter should have been called
}

TEST_F(UniqueBufferTest, Swap) {
    MockDeleter mock_deleter1;
    MockDeleter mock_deleter2;

    auto* ptr1 = new int[3];
    auto* ptr2 = new int[4];

    pt::memory::unique_buffer<int> buf1(ptr1, 3, std::move(mock_deleter1));
    pt::memory::unique_buffer<int> buf2(ptr2, 4, std::move(mock_deleter2));

    // Swap
    buf1.swap(buf2);

    EXPECT_EQ(buf1.size(), 4);
    EXPECT_EQ(buf2.size(), 3);
}

TEST_F(UniqueBufferTest, View) {
    auto* ptr = new int[5]{1, 2, 3, 4, 5};
    pt::memory::unique_buffer<int> buf(ptr, 5, [](int* p) { delete[] p; });

    auto view = buf.view();

    EXPECT_EQ(view.size(), 5);
    EXPECT_EQ(view.data(), ptr);
    EXPECT_EQ(view.size_bytes(), 5 * sizeof(int));
}

TEST_F(UniqueBufferTest, NullPointerWithZeroSize) {
    // Test with nullptr and zero size - this should work fine
    pt::memory::unique_buffer<int> buf(nullptr, 0, [](int*) {});

    EXPECT_FALSE(buf);
    EXPECT_EQ(buf.get(), nullptr);
    EXPECT_EQ(buf.size(), 0);
}

TEST_F(UniqueBufferTest, EmptyDeleter) {
    // Test with null deleter (should not crash)
    pt::memory::unique_buffer buf(new int[5], 5, {});

    EXPECT_TRUE(buf);

    // Should still work normally
    buf.reset();
    delete buf.data();
}

TEST_F(UniqueBufferTest, ExceptionSafetyInReset) {
    auto throwing_deleter = [](int* p) {
        delete[] p;
        throw std::runtime_error("Deleter error");
    };

    pt::memory::unique_buffer<int> buf(new int[3], 3, throwing_deleter);

    // Should not throw from destructor (caught internally)
    try {
        buf.reset();
    } catch (...) {
        FAIL() << "reset() should handle exceptions gracefully";
    }
}