#pragma once
#include "CommandBucket.h"

class CommandBuilder {
public:
    CommandBuilder(CommandBucket *cmdBucket) :cmdBucket(cmdBucket) { }
    // TODO include key generation
    // TODO include linking methods?

    Handle buildLoadVertexArrayCommand(Handle geometryBuffer,
        bool isIndexArray,
        void * geometryData,
        unsigned int elementSize,
        unsigned int arraySize);
    Handle buildCreateShaderCommand(Handle shaderProgram, const char * shaderSourceData, GPU::ShaderType shaderType);
    Handle buildSetShaderProgramCommand(Handle shaderProgramHandle);
private:
    CommandBucket *cmdBucket;
};