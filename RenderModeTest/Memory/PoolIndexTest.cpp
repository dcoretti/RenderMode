#include "gtest/gtest.h"
#include "Memory/PoolIndex.h"
TEST(PoolIndexTest, testCreate) {
    PoolIndex p(5);
    Handle h = p.createHandle();
    EXPECT_EQ(defaultHandleBaseVersion, h.version);
    EXPECT_EQ(nullptr, p.get(h));

    char c = 'a';
    Handle h2 = p.createHandle(&c);
    EXPECT_NE(h.index, h2.index);
    EXPECT_EQ(defaultHandleBaseVersion, h2.version);
    EXPECT_EQ(&c, (char*)p.get(h2));
}

TEST(PoolIndexTest, testIsValid) {
    PoolIndex p(5);
    Handle h = p.createHandle();
    EXPECT_TRUE(p.isValid(h));
    p.clear();
    EXPECT_FALSE(p.isValid(h));
}