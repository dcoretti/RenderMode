#include "gtest/gtest.h"

#include "Render/RenderQueue.h"
#include "Memory/Handle.h"
#include "Command\Command.h"

TEST(RenderQueueTest, testSubmit) {
    Handle cmd;
    CommandKey key;
    RenderQueue q(1);
    EXPECT_TRUE(q.isEmpty());
    EXPECT_TRUE(q.submit(&cmd, &key, 1));

    EXPECT_EQ(1, q.numCommands());
    EXPECT_FALSE(q.submit(&cmd, &key, 1));
}


void assertSetEqualSingleCmd(CommandSet *set, Handle h, CommandKey k) {
    EXPECT_NE(nullptr, set);
    EXPECT_EQ(1, set->numCommands);

    EXPECT_EQ(set->cmds[0].index, h.index);
    EXPECT_EQ(set->cmds[0].version, h.version);
    EXPECT_EQ(set->keys[0].tmp, k.tmp);
}

TEST(RenderQueueTest, testExecute) {
    Handle cmd;
    cmd.index = 123;

    CommandKey key;
    key.tmp = 5;

    RenderQueue q(1);
    EXPECT_TRUE(q.submit(&cmd, &key, 1));
    
    assertSetEqualSingleCmd(q.pop(), cmd, key);
}

TEST(RenderQueueTest, testPopOrder) {
    Handle cmd1;
    cmd1.index = 123;
    cmd1.version = 2;
    CommandKey key1;
    key1.tmp = 5;

    Handle cmd2;
    cmd2.index = 456;
    cmd2.version = 3;

    CommandKey key2;
    key2.tmp = 6;

    RenderQueue q(2);
    EXPECT_TRUE(q.submit(&cmd1, &key1, 1));
    EXPECT_TRUE(q.submit(&cmd2, &key2, 1));

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
    key1.tmp = 5;

    Handle cmd2;
    cmd2.index = 456;
    cmd2.version = 3;
    CommandKey key2;
    key2.tmp = 6;

    Handle cmd3;
    cmd3.index = 789;
    cmd2.version = 4;
    CommandKey key3;
    key3.tmp = 7;

    EXPECT_TRUE(q.submit(&cmd1, &key1, 1));
    EXPECT_TRUE(q.submit(&cmd2, &key2, 1));
    assertSetEqualSingleCmd(q.pop(), cmd1, key1);

    EXPECT_TRUE(q.submit(&cmd3, &key3, 1));
    assertSetEqualSingleCmd(q.pop(), cmd2, key2);
    assertSetEqualSingleCmd(q.pop(), cmd3, key3);

    EXPECT_TRUE(q.isEmpty());
}