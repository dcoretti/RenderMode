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
    CommandBuilder builder(bucket);

    Handle h;
    h.index = 123;
    h.version = 5;
    char * data = "hi";
    //Handle cmdHandle = builder.buildCreateShaderCommand(h, data, GPU::ShaderType::Fragment);

    //Command *cmd =  bucket->getCommand(cmdHandle);
    //CreateShaderCommand * cmdData = bucket->getCommandData<CreateShaderCommand>(cmdHandle);

}