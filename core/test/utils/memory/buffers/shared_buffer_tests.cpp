#include <gtest/gtest.h>
#include <utils/memory/smart_buffers/shared_buffer.hpp>
#include "../MockDeleter.hpp"

class SharedBufferTest : public ::testing::Test {
protected:
    void SetUp() override {}

    void TearDown() override {}
};

// Test basic construction
TEST_F(SharedBufferTest, DefaultConstruction) {
    pt::memory::shared_buffer<int> sb;

    EXPECT_FALSE(sb);
    EXPECT_EQ(sb.use_count(), 0);
    EXPECT_EQ(sb.data(), nullptr);
    EXPECT_EQ(sb.size(), 0);
}

// Test construction with pointer/deleter
TEST_F(SharedBufferTest, ConstructionWithPointer) {
    MockDeleter mock_deleter;
    auto* ptr = new int[10];

    pt::memory::shared_buffer<int> sb(ptr, 10, mock_deleter);

    EXPECT_TRUE(sb);
    EXPECT_EQ(sb.use_count(), 1);
    EXPECT_NE(sb.data(), nullptr);
    EXPECT_EQ(sb.size(), 10);
}

// Test copy construction
TEST_F(SharedBufferTest, CopyConstruction) {
    MockDeleter mock_deleter;
    auto* ptr = new int[5];

    pt::memory::shared_buffer<int> sb1(ptr, 5, mock_deleter);
    EXPECT_EQ(sb1.use_count(), 1);

    pt::memory::shared_buffer sb2(sb1);
    EXPECT_EQ(sb1.use_count(), 2);
    EXPECT_EQ(sb2.use_count(), 2);
}

// Test copy assignment
TEST_F(SharedBufferTest, CopyAssignment) {
    MockDeleter mock_deleter1;
    MockDeleter mock_deleter2;

    auto* ptr1 = new int[3];
    auto* ptr2 = new int[4];

    pt::memory::shared_buffer<int> sb1(ptr1, 3, mock_deleter1);
    pt::memory::shared_buffer<int> sb2(ptr2, 4, mock_deleter2);

    EXPECT_EQ(sb1.use_count(), 1);
    EXPECT_EQ(sb2.use_count(), 1);

    sb2 = sb1;  // Copy assignment
    EXPECT_TRUE(MockDeleter::deleted);
    MockDeleter::deleted = false;

    EXPECT_EQ(sb1.use_count(), 2);
    EXPECT_EQ(sb2.use_count(), 2);
}

// Test move construction
TEST_F(SharedBufferTest, MoveConstruction) {
    MockDeleter mock_deleter;
    auto* ptr = new int[5];

    pt::memory::shared_buffer<int> sb1(ptr, 5, mock_deleter);
    EXPECT_EQ(sb1.use_count(), 1);

    pt::memory::shared_buffer<int> sb2(std::move(sb1));
    EXPECT_FALSE(sb1);  // Should be empty after move
    EXPECT_TRUE(sb2);
    EXPECT_EQ(sb2.use_count(), 1);
}

// Test move assignment
TEST_F(SharedBufferTest, MoveAssignment) {
    MockDeleter mock_deleter1;
    MockDeleter mock_deleter2;
    auto* ptr1 = new int[3];
    auto* ptr2 = new int[4];

    pt::memory::shared_buffer<int> sb1(ptr1, 3, mock_deleter1);
    pt::memory::shared_buffer<int> sb2(ptr2, 4, mock_deleter2);

    EXPECT_EQ(sb1.use_count(), 1);
    EXPECT_EQ(sb2.use_count(), 1);

    sb2 = std::move(sb1);  // Move assignment

    EXPECT_FALSE(sb1);  // Should be empty after move
    EXPECT_TRUE(sb2);
    EXPECT_EQ(sb2.use_count(), 1);
}

// Test release functionality
TEST_F(SharedBufferTest, ReleaseUniqueOwner) {
    MockDeleter mock_deleter;
    auto* ptr = new int[5];

    pt::memory::shared_buffer<int> sb(ptr, 5, mock_deleter);

    // Should succeed since we're the only owner
    auto released = sb.release();
    EXPECT_TRUE(released.has_value());
    EXPECT_FALSE(sb);  // Should be empty now

    // Verify the released buffer has correct properties
    auto& rb = released.value();
    EXPECT_EQ(rb.ptr, ptr);
    EXPECT_EQ(rb.count, 5);

    // Clean up
    rb.deleter(rb.ptr);
}

// Test release when not unique owner
TEST_F(SharedBufferTest, ReleaseNonUniqueOwner) {
    MockDeleter mock_deleter;
    auto* ptr = new int[5];

    pt::memory::shared_buffer<int> sb1(ptr, 5, mock_deleter);
    pt::memory::shared_buffer sb2(sb1);  // Copy

    EXPECT_EQ(sb1.use_count(), 2);

    auto released = sb1.release();
    EXPECT_FALSE(released.has_value());  // Should fail
    EXPECT_TRUE(sb1);  // Still valid since we're not the unique owner
}

// Test swap functionality
TEST_F(SharedBufferTest, Swap) {
    MockDeleter mock_deleter1;
    MockDeleter mock_deleter2;

    auto* ptr1 = new int[3];
    auto* ptr2 = new int[4];

    pt::memory::shared_buffer<int> sb1(ptr1, 3, mock_deleter1);
    pt::memory::shared_buffer<int> sb2(ptr2, 4, mock_deleter2);

    sb1.swap(sb2);

    EXPECT_EQ(sb1.size(), 4);
    EXPECT_EQ(sb2.size(), 3);
}

// Test view functionality
TEST_F(SharedBufferTest, View) {
    MockDeleter mock_deleter;
    auto* ptr = new int[5]{1, 2, 3, 4, 5};

    pt::memory::shared_buffer<int> sb(ptr, 5, mock_deleter);

    auto view = sb.view();
    EXPECT_EQ(view.size(), 5);
    EXPECT_EQ(view.data(), ptr);
}

// Test span functionality
TEST_F(SharedBufferTest, Span) {
    MockDeleter mock_deleter;
    auto* ptr = new int[3]{10, 20, 30};

    pt::memory::shared_buffer<int> sb(ptr, 3, mock_deleter);

    auto span = sb.span();
    EXPECT_EQ(span.size(), 3);
    EXPECT_EQ(span[0], 10);
    EXPECT_EQ(span[1], 20);
    EXPECT_EQ(span[2], 30);
}

 // Test with unique_buffer conversion
TEST_F(SharedBufferTest, UniqueBufferConversion) {
    MockDeleter mock_deleter;
    MockDeleter::deleted = false;

    {
        auto* ptr = new int[5]{1, 2, 3, 4, 5};

        pt::memory::unique_buffer<int> ub(ptr, 5, mock_deleter);

        // Convert to shared buffer
        pt::memory::shared_buffer sb(std::move(ub));

        EXPECT_TRUE(sb);
        EXPECT_EQ(sb.size(), 5);
        EXPECT_FALSE(ub); // Should be empty now

        // Test that we can still access the data
        auto span = sb.span();
        EXPECT_EQ(span[0], 1);
        EXPECT_EQ(span[4], 5);

        // The original deleter should NOT have been called yet
        EXPECT_FALSE(MockDeleter::deleted);
    }

    EXPECT_TRUE(MockDeleter::deleted);
}

// Test empty shared_buffer handling
TEST_F(SharedBufferTest, EmptyBufferHandling) {
    pt::memory::shared_buffer<int> sb;
    EXPECT_FALSE(sb);
    EXPECT_EQ(sb.use_count(), 0);
    EXPECT_EQ(sb.data(), nullptr);
    EXPECT_EQ(sb.size(), 0);

    auto released = sb.release();
    EXPECT_FALSE(released.has_value());
}