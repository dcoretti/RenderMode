#pragma once
#include "CommandBucket.h"

#include "../Types/Application/Model.h"
#include "../Types/Application/TransientModelData.h"

// TODO include key generation
// TODO include linking methods?

/*
    Command builder adapts application level objects (meshes, models, materials) to render engine commands.
    At this level no pools or handles are used as input to the public API, only actual objects to avoid knowledge of pools

    Two levels of building are done here: high level transformation of application types to commands (eg loading meshes), and mid level transformation to commands (eg loading vertex arrays)

*/
class CommandBuilder {
public:
    CommandBuilder(CommandBucket *cmdBucket) :cmdBucket(cmdBucket) { }

    // TODO add sort key as param here?
    Handle buildDrawModelCommand(const Model & mesh);
    Handle buildLoadModelCommand(Model & model, TransientModelData &modelData);




    Handle buildLoadVertexArrayCommand(Handle geometryBuffer,
        bool isIndexArray,
        void * geometryData,
        unsigned int elementSize,
        unsigned int arraySize);
    Handle buildLoadVertexArrayCommandWithParent(Handle geometryBuffer,
        bool isIndexArray,
        void * geometryData,
        unsigned int elementSize,
        unsigned int arraySize, 
        Handle parentCommand);
    Handle buildCreateShaderCommand(Handle shaderProgram, const char * shaderSourceData, GPU::ShaderType shaderType);
    Handle buildSetShaderProgramCommand(Handle shaderProgramHandle);
private:
    CommandBucket *cmdBucket;
};