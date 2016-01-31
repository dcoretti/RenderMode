#include "gtest/gtest.h"
#include "Memory\PackedArray.h"

TEST(PackedArrayTest, testArray) {
    int elements = 5;
    PackedArray<char> arr(elements);
    char * last;
    bool first = true;
    for (int i = 0; i < elements; i++) {
        if (first) {
            last = arr.get(0);
            first = false;
        }

        EXPECT_EQ(i, arr.allocate());
        EXPECT_EQ(last + i, arr.get(i));
    }
}


TEST(PackedArrayTest, testDeleteSingle) {
    PackedArray<char> arr(1);
    int i = arr.allocate();
    EXPECT_EQ(-1, arr.remove(i));
}

TEST(PackedArrayTest, testDeleteMulti) {
    PackedArray<char> arr(3);
    int i = arr.allocate();
    int i2 = arr.allocate();
    int i3 = arr.allocate();
    EXPECT_EQ(i3, arr.remove(i2));
}

TEST(PackedArrayTest, testDeleteMultiRealloc) {
    PackedArray<char> arr(4);
    int i = arr.allocate();
    int i2 = arr.allocate();
    int i3 = arr.allocate();
    EXPECT_EQ(i3, arr.remove(i2));
    int i4 = arr.allocate();
    EXPECT_EQ(i3, i4);  // i3 was moved so i4 should have the same index.
    EXPECT_EQ(i4+1, arr.allocate());
}