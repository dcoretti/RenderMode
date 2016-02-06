#include "CommandBuilder.h"
#include "../Types/Application/Mesh.h"

//////////////////////////////////////////////////////////////////////////////////////
//  High-level transformation
//  

Handle CommandBuilder::buildDrawModelCommand(const Model & mesh) {
    return Handle();
}

Handle CommandBuilder::buildLoadModelCommand(Model & model, TransientModelData &modelData) {
    Handle loadVertexCmd = buildLoadVertexArrayCommand(model.vertices, 
        false, 
        modelData.vertices, 
        modelData.vertexComponents * modelData.vertexElementSize, 
        modelData.elements);
    Handle loadNormalCmd = buildLoadVertexArrayCommandWithParent(model.normals, 
        false, 
        modelData.normals, 
        modelData.normalComponents * modelData.vertexElementSize, 
        modelData.elements, 
        loadVertexCmd);
    Handle loadTexCmd = buildLoadVertexArrayCommandWithParent(model.texCoords, 
        false, 
        modelData.texCoords, 
        modelData.texCoordComponents * modelData.vertexElementSize, 
        modelData.elements, 
        loadVertexCmd);
    Handle loadIndexCmd = buildLoadVertexArrayCommandWithParent(model.indices,
        false,
        modelData.indices,
        1 * modelData.indexElementSize,
        modelData.elements,
        loadVertexCmd);

    return loadVertexCmd;
}


//////////////////////////////////////////////////////////////////////////////////////
//  Mid-level transforamtion of data to commands
//

Handle CommandBuilder::buildLoadVertexArrayCommandWithParent(Handle geometryBuffer,
    bool isIndexArray,
    void * geometryData,
    unsigned int elementSize,
    unsigned int arraySize,
    Handle parentCommand) {
    Handle loadVertexCmdHandle = cmdBucket->createCommand<LoadArrayBufferCommand>(parentCommand);
    LoadArrayBufferCommand * cmd = cmdBucket->getCommandData<LoadArrayBufferCommand>(loadVertexCmdHandle);
    cmd = new (cmd) LoadArrayBufferCommand(geometryBuffer, isIndexArray, geometryData, elementSize, arraySize);
    return loadVertexCmdHandle;
}

Handle CommandBuilder::buildLoadVertexArrayCommand(Handle geometryBuffer,
                                                bool isIndexArray,
                                                void * geometryData,
                                                unsigned int elementSize,
                                                unsigned int arraySize) {
    Handle loadVertexCmdHandle = cmdBucket->createCommand<LoadArrayBufferCommand>(CommandKey());
    LoadArrayBufferCommand * cmd = cmdBucket->getCommandData<LoadArrayBufferCommand>(loadVertexCmdHandle);
    cmd = new (cmd) LoadArrayBufferCommand(geometryBuffer, isIndexArray, geometryData, elementSize, arraySize);

    return loadVertexCmdHandle;
}


Handle CommandBuilder::buildCreateShaderCommand(Handle shaderProgram, const char * shaderSourceData, GPU::ShaderType shaderType) {
    Handle createShaderCmdHandle = cmdBucket->createCommand<CreateShaderCommand>(CommandKey());
    CreateShaderCommand * cmd = cmdBucket->getCommandData<CreateShaderCommand>(createShaderCmdHandle);
    cmd = new (cmd) CreateShaderCommand(shaderProgram, shaderSourceData, shaderType);

    return createShaderCmdHandle;
}


Handle CommandBuilder::buildSetShaderProgramCommand(Handle shaderProgramHandle) {
    Handle setShaderCmdHandle = cmdBucket->createCommand<SetShaderProgramCommand>(CommandKey());
    SetShaderProgramCommand * cmd = cmdBucket->getCommandData<SetShaderProgramCommand>(setShaderCmdHandle);
    cmd = new (cmd) SetShaderProgramCommand(shaderProgramHandle);

    return setShaderCmdHandle;
}
