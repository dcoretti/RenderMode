#include "gtest/gtest.h"

#include "Types\GPU\GeometryTypes.h"
#include "Memory\LinearAllocator.h"
#include "Command\CommandBucket.h"
#include "Command\CommandBuilder.h"


class CommandBuilderTest : public testing::Test {
protected:
    virtual void SetUp() {
        bucket = new CommandBucket(5, 500);
    }

    virtual void TearDown() {
        delete bucket;
    }

    CommandBucket *bucket;
};

TEST_F(CommandBuilderTest, CreateShaderProgramTest) {
}