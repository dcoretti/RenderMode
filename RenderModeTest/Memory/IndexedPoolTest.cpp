#include "gtest/gtest.h"

#include "Memory\IndexedPool.h"

void generateEntries(IndexedPool<char> &pool, Handle * handles, int size, char startChar = 'a') {
    for (int i = 0; i < size; i++) {
        handles[i] = pool.createObject();
        *pool.get(handles[i]) = startChar + i;
    }
}


TEST(IndexedPoolTest, testPoolHandle) {
    IndexedPool<char> pool(5);
    Handle h = pool.createObject();
    EXPECT_EQ(0, h.index);
    EXPECT_EQ(defaultHandleBaseVersion, h.version);

    char * c = pool.get(h);
    EXPECT_NE(nullptr, c);
}

TEST(IndexedPoolTest, testCrapHandle) {
    IndexedPool<char> pool(5);
    Handle h = pool.createObject();
    h.index = 1234;
    EXPECT_EQ(nullptr, pool.get(h));
}


TEST(IndexedPoolTest, testManyObj) {
    int num = 5;
    IndexedPool<char> pool(5);
    Handle last = pool.createObject();
    for (int i = 0; i < num-1; i++) {
        Handle h = pool.createObject();
        EXPECT_NE(last.index, h.index);
        EXPECT_EQ(last.version, h.version);
    }
}

TEST(IndexedPoolTest, testDelete) {
    IndexedPool<char> pool(1);
    Handle h = pool.createObject();
    EXPECT_EQ(0, h.index);
    EXPECT_EQ(defaultHandleBaseVersion, h.version);
    pool.deleteObject(h);
    EXPECT_EQ(nullptr, pool.get(h));
    Handle h2 = pool.createObject();

    EXPECT_EQ(h.index, h2.index);
    EXPECT_NE(h.version, h2.version);
    EXPECT_EQ(nullptr, pool.get(h));
}

TEST(IndexedPoolTest, testDeleteWithMove) {
    const int poolSize = 3;
    IndexedPool<char> pool(poolSize);
    Handle handles[poolSize];
    generateEntries(pool, handles, poolSize);

    pool.deleteObject(handles[1]);  // h3 should be moved to h2's data slot as a result
    EXPECT_EQ(nullptr, pool.get(handles[1]));

    Handle h2 = pool.createObject();
    *pool.get(h2) = 'z';

    EXPECT_EQ('a', *pool.get(handles[0]));
    EXPECT_EQ('c', *pool.get(handles[2]));
    EXPECT_EQ('z', *pool.get(h2));
}

TEST(IndexedPoolTest, testClear) {
    const int poolSize = 3;
    IndexedPool<char> pool(poolSize);
    Handle handles[poolSize];
    generateEntries(pool, handles, poolSize);

    EXPECT_EQ(poolSize, pool.getSize());
    pool.clear();
    EXPECT_EQ(0, pool.getSize());
    for (int i = 0; i < poolSize; i++) {
        // All handles are now invalid as we have a base version for all items that has changed.
        EXPECT_EQ(nullptr, pool.get(handles[i]));
    }
}


TEST(IndexedPoolTest, testUnderlyingStructAssumptions) {
    // Basic assumption of sparse array zero-initializing the list.  
    //Required for the pool-index to function properly
    SparseArray<Handle> arr(3);
    EXPECT_EQ(0, arr.get(0)->version);
    EXPECT_EQ(0, arr.get(0)->index);
}