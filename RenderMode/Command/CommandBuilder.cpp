#include "CommandBuilder.h"



Handle CommandBuilder::buildLoadVertexArrayCommand(Handle geometryBuffer,
                                                bool isIndexArray,
                                                void * geometryData,
                                                unsigned int elementSize,
                                                unsigned int arraySize) {
    Handle loadVertexCmdHandle = cmdBucket->createCommand<LoadArrayBufferCommand>(CommandKey());
    LoadArrayBufferCommand * cmd = cmdBucket->getCommandData<LoadArrayBufferCommand>(loadVertexCmdHandle);

    cmd->elementSize = elementSize;
    cmd->geometryBuffer = geometryBuffer;
    cmd->systemBuffer = geometryData;
    cmd->systemBufferSize = elementSize * arraySize;
    cmd->isIndexArray = isIndexArray;

    return loadVertexCmdHandle;
}


Handle CommandBuilder::buildCreateShaderCommand(Handle shaderProgram, const char * shaderSourceData, GPU::ShaderType shaderType) {
    Handle createShaderCmdHandle = cmdBucket->createCommand<CreateShaderCommand>(CommandKey());
    CreateShaderCommand * cmd = cmdBucket->getCommandData<CreateShaderCommand>(createShaderCmdHandle);

    cmd->shaderSourceData = shaderSourceData;
    cmd->shaderType = shaderType;
    cmd->shaderProgram = shaderProgram;

    return createShaderCmdHandle;
}


Handle CommandBuilder::buildSetShaderProgramCommand(Handle shaderProgramHandle) {
    Handle setShaderCmdHandle = cmdBucket->createCommand<SetShaderProgramCommand>(CommandKey());
    SetShaderProgramCommand * cmd = cmdBucket->getCommandData<SetShaderProgramCommand>(setShaderCmdHandle);
    cmd->shaderProgramHandle = shaderProgramHandle;

    return setShaderCmdHandle;
}
