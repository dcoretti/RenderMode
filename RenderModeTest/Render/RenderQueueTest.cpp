#include "gtest/gtest.h"

#include "Render/RenderQueue.h"
#include "Memory/Handle.h"
#include "Command\Command.h"

TEST(RenderQueueTest, testSubmit) {
    Handle cmd;
    cmd.index = 0;
    cmd.version = 0;
    CommandKey key;
    key.materialId = 1;
    RenderQueue q(1);
    EXPECT_TRUE(q.isEmpty());
    EXPECT_TRUE(q.submit(cmd, key));

    EXPECT_EQ(1, q.numCommands());
    EXPECT_FALSE(q.submit(cmd, key));
}


void assertSetEqualSingleCmd(Handle actual, Handle expectedHandle, CommandKey k) {
    EXPECT_EQ(actual.index, expectedHandle.index);
    EXPECT_EQ(actual.version, expectedHandle.version);
}

TEST(RenderQueueTest, testExecute) {
    Handle cmd;
    cmd.index = 123;

    CommandKey key;
    key.materialId = 5;

    RenderQueue q(1);
    EXPECT_TRUE(q.submit(cmd, key));
    
    assertSetEqualSingleCmd(q.pop(), cmd, key);
}

TEST(RenderQueueTest, testPopOrder) {
    Handle cmd1;
    cmd1.index = 123;
    cmd1.version = 2;
    CommandKey key1;
    key1.materialId = 5;

    Handle cmd2;
    cmd2.index = 456;
    cmd2.version = 3;

    CommandKey key2;
    key2.materialId = 6;

    RenderQueue q(2);
    EXPECT_TRUE(q.submit(cmd1, key1));
    EXPECT_TRUE(q.submit(cmd2, key2));

    assertSetEqualSingleCmd(q.pop(), cmd1, key1);
    EXPECT_EQ(1, q.numCommands());
    assertSetEqualSingleCmd(q.pop(), cmd2, key2);
    EXPECT_EQ(0, q.numCommands());
    EXPECT_TRUE(q.isEmpty());
}

TEST(RenderQueueTest, testWrap) {
    RenderQueue q(2);

    Handle cmd1;
    cmd1.index = 123;
    cmd1.version = 2;
    CommandKey key1;
    key1.materialId = 5;

    Handle cmd2;
    cmd2.index = 456;
    cmd2.version = 3;
    CommandKey key2;
    key2.materialId = 6;

    Handle cmd3;
    cmd3.index = 789;
    cmd2.version = 4;
    CommandKey key3;
    key3.materialId = 7;

    EXPECT_TRUE(q.submit(cmd1, key1));
    EXPECT_TRUE(q.submit(cmd2, key2));
    assertSetEqualSingleCmd(q.pop(), cmd1, key1);

    EXPECT_TRUE(q.submit(cmd3, key3));
    assertSetEqualSingleCmd(q.pop(), cmd2, key2);
    assertSetEqualSingleCmd(q.pop(), cmd3, key3);

    EXPECT_TRUE(q.isEmpty());
}