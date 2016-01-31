#include "gtest/gtest.h"

#include "Types\GPU\GeometryTypes.h"
#include "Memory\LinearAllocator.h"
#include "Command\CommandBucket.h"
#include "Command\CommandBuilder.h"


class CommandBuilderTest : public testing::Test {
protected:
    virtual void SetUp() {
        cmdPool = new LinearAllocator(1024*5);
        bucket = new CommandBucket(cmdPool, 10);
    }

    virtual void TearDown() {
        delete bucket;
        delete cmdPool;
    }

    CommandBucket *bucket;
    LinearAllocator *cmdPool;
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