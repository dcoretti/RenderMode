#include "gtest/gtest.h"
#include "Memory\LinearAllocator.h"

TEST(LinearAllocatorTest, testSize) {
    LinearAllocator alloc(10);
    EXPECT_EQ(10, alloc.maxSize());
    EXPECT_EQ(nullptr, alloc.alloc(0));
    EXPECT_NE(nullptr, alloc.alloc(1));
    EXPECT_EQ(1, alloc.curSize());
    EXPECT_NE(nullptr, alloc.alloc(1));
    EXPECT_EQ(2, alloc.curSize());

    alloc.clear();
    EXPECT_EQ(0, alloc.curSize());
    EXPECT_EQ(nullptr, alloc.alloc(11));
}

TEST(LinearAllocatorTest, testPlacement) {
    LinearAllocator alloc(10);
    char * hi = new (alloc) char;
    EXPECT_EQ(1, alloc.curSize());

    EXPECT_EQ(hi + 1, alloc.alloc(1));
    EXPECT_EQ(hi + 2, new (alloc) char);
}


TEST(LinearAllocatorTest, testVaryingSize) {
    LinearAllocator allocator(10);
    allocator.alloc(3);
    allocator.alloc(5);
    EXPECT_EQ(8, allocator.curSize());
}