#include "gtest/gtest.h"
#include "Command/CommandBucket.h"
#include "Command/Command.h"

class CommandBucketTest: public testing::Test {
protected:
    virtual void SetUp() {
        b = new CommandBucket(5, 500);
    }

    virtual void TearDown() {
        delete b;
    }

    CommandBucket *b;
};

TEST_F(CommandBucketTest, testCreateCommand) {
    EXPECT_EQ(0, b->getPoolSize());

    Handle h = b->createCommand<CreateShaderCommand>(CommandKey());
    Command * cmd = b->getCommand(h);
    CreateShaderCommand *cmdData = b->getCommandData<CreateShaderCommand>(h);
    EXPECT_EQ(sizeof(Command) + sizeof(CreateShaderCommand), b->getPoolSize());
    EXPECT_NE(nullptr, cmd);
    EXPECT_NE(nullptr, cmdData);
    EXPECT_EQ(1, b->getNumSortableCommands());
    EXPECT_EQ(1, b->getTotalCommands());

}

TEST_F(CommandBucketTest, testCreateCommandWithParent) {
    EXPECT_EQ(0, b->getPoolSize());
    EXPECT_EQ(0, b->getNumSortableCommands());
    EXPECT_EQ(0, b->getTotalCommands());

    Handle parent = b->createCommand<CreateShaderCommand>(CommandKey());
    EXPECT_EQ(sizeof(Command) + sizeof(CreateShaderCommand), b->getPoolSize());
    EXPECT_EQ(1, b->getNumSortableCommands());
    EXPECT_EQ(1, b->getTotalCommands());

    Handle child = b->createCommand<SetShaderProgramCommand>(parent);
    EXPECT_EQ(sizeof(Command) * 2 + sizeof(CreateShaderCommand) + sizeof(SetShaderProgramCommand), b->getPoolSize());
    EXPECT_EQ(1, b->getNumSortableCommands());
    EXPECT_EQ(2, b->getTotalCommands());

    Command * parentCmd = b->getCommand(parent);
    Command * childCmd = b->getCommand(child);
    EXPECT_EQ(childCmd, parentCmd->next);
    EXPECT_EQ(nullptr, childCmd->next);
}