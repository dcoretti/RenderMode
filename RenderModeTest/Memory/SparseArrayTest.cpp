#include "gtest/gtest.h"
#include "Memory/SparseArray.h"

TEST(SparseArrayTest, testAdd) {
    SparseArray<char> arr(2);
    EXPECT_EQ(0, arr.getNumElements());
    int index = arr.allocate();
    EXPECT_EQ(1, arr.getNumElements());

    int index2 = arr.allocate();
    EXPECT_EQ(2, arr.getNumElements());
    EXPECT_NE(index, index2);
    // no more space
    EXPECT_EQ(-1, arr.allocate());
}

TEST(SparseArrayTest, testRemove) {
    SparseArray<char> arr(2);
    EXPECT_EQ(0, arr.getNumElements());
    int index = arr.allocate();
    EXPECT_EQ(1, arr.getNumElements());
    arr.remove(index);
    EXPECT_EQ(0, arr.getNumElements());
    index = arr.allocate();
    EXPECT_EQ(1, arr.getNumElements());
    int index2 = arr.allocate();
    EXPECT_EQ(2, arr.getNumElements());
    EXPECT_EQ(-1, arr.allocate());
}

TEST(SparseArrayTest, testSingleFree) {
    SparseArray<char> arr(3);
    EXPECT_EQ(0, arr.getNumElements());
    int index = arr.allocate();
    int index2 = arr.allocate();
    EXPECT_EQ(2, arr.getNumElements());
    arr.remove(index);
    EXPECT_EQ(1, arr.getNumElements());

    int index3 = arr.allocate();
    EXPECT_EQ(index, index3);
    EXPECT_EQ(2, arr.getNumElements());
}

TEST(SparseArrayTest, testMultiFree) {
    SparseArray<char> arr(3);
    EXPECT_EQ(0, arr.getNumElements());
    int index = arr.allocate();
    int index2 = arr.allocate();
    int index3 = arr.allocate();
    EXPECT_EQ(3, arr.getNumElements());
    arr.remove(index);
    arr.remove(index2);
    EXPECT_EQ(1, arr.getNumElements());

    int index4 = arr.allocate();
    EXPECT_EQ(2, arr.getNumElements());
    EXPECT_EQ(index2, index4);  // last deleted first allocated

    int index5 = arr.allocate();
    EXPECT_EQ(3, arr.getNumElements());
    EXPECT_EQ(index, index5);  // last deleted first allocated
}


TEST(SparseArrayTest, testClear) {
    SparseArray<char> arr(3);
    for (int i = 0; i < 3; i++) {
        arr.allocate();
    }
    EXPECT_EQ(3, arr.getNumElements());
    arr.clear();
    EXPECT_EQ(0, arr.getNumElements());
    int index = arr.allocate();
    EXPECT_EQ(0, index);
}

TEST(SparseArrayTest, testClearWithFreeList) {
    SparseArray<char> arr(3);
    int indices[3];
    for (int i = 0; i < 3; i++) {
        indices[i] = arr.allocate();
    }
    EXPECT_EQ(3, arr.getNumElements());
    arr.remove(indices[1]);
    EXPECT_EQ(2, arr.getNumElements());

    arr.clear();
    EXPECT_EQ(0, arr.getNumElements());
    int index = arr.allocate();
    EXPECT_EQ(0, index);
}