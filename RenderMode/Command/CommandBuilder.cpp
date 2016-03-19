#include "CommandBuilder.h"
#include "../Types/Application/Mesh.h"

//////////////////////////////////////////////////////////////////////////////////////
//  High-level transformation
//  

Handle CommandBuilder::buildDrawModelCommand(const Model & mesh) {
    return Handle();
}

Handle CommandBuilder::buildLoadModelCommand(Model & model, TransientModelData &modelData) {
    // fill out model layout information using transient model data
    Handle initVaoCmd = buildInitializeAndSetVertexArrayCommand(model.vao);

    GPU::GeometryBufferLayout *vertexLayout = renderContext->geometryBufferLayoutPool.get(model.vertexLayout);
    vertexLayout->numComponents = modelData.vertexComponents;

    Handle loadVertexHandle = buildLoadVertexArrayCommandWithParent(modelData.vertices,
            model.vertices,
            GPU::ShaderAttributeBinding::VERTICES,
            *renderContext->geometryBufferLayoutPool.get(model.vertexLayout),
            initVaoCmd);


    // FIX
    // Do i need to set bools on model to denote if optional data is set???? can I be sure the handles 
    // are validated when drawing to ensure we don't draw the normals if they don't exist?
    // doesn't matter- VAO contains all the info so i won't need to use the handles unless updating.
    if (modelData.normals.data != nullptr) {
        GPU::GeometryBufferLayout *normalsLayout = renderContext->geometryBufferLayoutPool.get(model.normalsLayout);
        normalsLayout->numComponents = modelData.normalComponents;

        Handle loadNormalsHandle = buildLoadVertexArrayCommandWithParent(modelData.normals,
            model.normals,
            GPU::ShaderAttributeBinding::NORMALS,
            *renderContext->geometryBufferLayoutPool.get(model.normalsLayout),
            initVaoCmd);
    }
    if (modelData.texCoords.data != nullptr) {
        GPU::GeometryBufferLayout *texCoordsLayout = renderContext->geometryBufferLayoutPool.get(model.texCoordsLayout);
        texCoordsLayout->numComponents = modelData.texCoordComponents;
        Handle loadTexCoordsCommand = buildLoadVertexArrayCommandWithParent(
            modelData.texCoords,
            model.texCoords,
            GPU::ShaderAttributeBinding::UV,
            *renderContext->geometryBufferLayoutPool.get(model.texCoordsLayout),
            initVaoCmd);
    }
    if (modelData.indices.data != nullptr) {
        // TODO
    }

    return initVaoCmd;
}


//////////////////////////////////////////////////////////////////////////////////////
//  Mid-level transforamtion of data to commands
//
//
Handle CommandBuilder::buildLoadVertexArrayCommandWithParent(SystemBuffer systemBuffer,
    Handle geometryBuffer,
    GPU::ShaderAttributeBinding shaderBinding,
    GPU::GeometryBufferLayout bufferLayout,
    Handle parentCommand) {
    Handle loadVertexCmdHandle = cmdBucket->createCommand<LoadArrayBufferCommand>(parentCommand);
    LoadArrayBufferCommand * cmd = cmdBucket->getCommandData<LoadArrayBufferCommand>(loadVertexCmdHandle);
    cmd = new (cmd) LoadArrayBufferCommand(systemBuffer, geometryBuffer, shaderBinding, bufferLayout);
    return loadVertexCmdHandle;
}

Handle CommandBuilder::buildLoadIndexArrayCommandWithParent(SystemBuffer systemBuffer, Handle geometryBuffer, Handle parentCommand) {
    Handle loadIndexCmdHandle = cmdBucket->createCommand<LoadIndexArrayBufferCommand>(parentCommand);
    LoadIndexArrayBufferCommand *cmd = cmdBucket->getCommandData<LoadIndexArrayBufferCommand>(loadIndexCmdHandle);

    cmd->indexGeometryBuffer = geometryBuffer;
    cmd->systemBuffer = systemBuffer;
    return loadIndexCmdHandle;
}

Handle CommandBuilder::buildLoadTextureWithParent(SystemBuffer systemBuffer, GPU::TextureBufferLayout textureBufferLayout, Handle textureBuffer, Handle parentCommand) {
    Handle loadTextureCmdHandle = cmdBucket->createCommand<LoadTextureBufferCommand>(parentCommand);
    LoadTextureBufferCommand *cmd = cmdBucket->getCommandData<LoadTextureBufferCommand>(loadTextureCmdHandle);

    cmd->systemBuffer = systemBuffer;
    cmd->textureBuffer = textureBuffer;
    cmd->textureBufferLayout = textureBufferLayout;
    return loadTextureCmdHandle;
}


Handle CommandBuilder::buildDrawArraysCommand(GPU::VertexArrayObject &vao, GPU::DrawContext &context) {
    Handle drawArrayCmdHandle = cmdBucket->createCommand<DrawVertexArrayCommand>(CommandKey());

    DrawVertexArrayCommand *cmd = cmdBucket->getCommandData<DrawVertexArrayCommand>(drawArrayCmdHandle);
    cmd = new (cmd) DrawVertexArrayCommand(false, vao, context);
    return drawArrayCmdHandle;
}

Handle CommandBuilder::buildDrawArraysCommandWithParent(GPU::VertexArrayObject &vao, GPU::DrawContext &context, Handle parentCommand) {
    Handle drawArrayCmdHandle = cmdBucket->createCommand<DrawVertexArrayCommand>(parentCommand);

    DrawVertexArrayCommand *cmd = cmdBucket->getCommandData<DrawVertexArrayCommand>(drawArrayCmdHandle);
    cmd = new (cmd) DrawVertexArrayCommand(false, vao, context);
    return drawArrayCmdHandle;
}


Handle CommandBuilder::buildDrawIndexedCommand(GPU::VertexArrayObject &vao, GPU::DrawContext &indexContext) {
    Handle drawArrayCmdHandle = cmdBucket->createCommand<DrawVertexArrayCommand>(CommandKey());

    DrawVertexArrayCommand *cmd = cmdBucket->getCommandData<DrawVertexArrayCommand>(drawArrayCmdHandle);
    cmd = new (cmd) DrawVertexArrayCommand(true, vao, indexContext);
    return drawArrayCmdHandle;
}


Handle CommandBuilder::buildInitializeAndSetVertexArrayCommand(Handle &vao) {
    Handle initVaoCommand = cmdBucket->createCommand<InitializeAndSetVertexArrayCommand>(CommandKey());
    InitializeAndSetVertexArrayCommand *cmd = cmdBucket->getCommandData<InitializeAndSetVertexArrayCommand>(initVaoCommand);
    cmd->vertexArrayObject = vao;

    return initVaoCommand;
}

Handle CommandBuilder::buildSetShaderProgramCommand(GPU::ShaderProgram shaderProgram) {
    Handle setShaderCmdHandle = cmdBucket->createCommand<SetShaderProgramCommand>(CommandKey());
    SetShaderProgramCommand *cmd = cmdBucket->getCommandData<SetShaderProgramCommand>(setShaderCmdHandle);

    cmd->shaderProgram = shaderProgram;
    return setShaderCmdHandle;
}

Handle CommandBuilder::buildCreateShaderCommand(GPU::ShaderData vertexShader,
    GPU::ShaderData fragmentShader,
    Handle shaderProgram) {
    Handle createCmdHandle = cmdBucket->createCommand<CreateShaderCommand>(CommandKey());
    CreateShaderCommand * cmd = cmdBucket->getCommandData<CreateShaderCommand>(createCmdHandle);
    cmd = new (cmd) CreateShaderCommand(shaderProgram, vertexShader, fragmentShader);
    
    return createCmdHandle;
}

Handle CommandBuilder::buildSetMatrixUniformCommand(int shaderUniform, void* matrixData, int numMatrices) {
    Handle setMatrixCmdHandle = cmdBucket->createCommand<SetMatrixUniformCommand>(CommandKey());
    SetMatrixUniformCommand * cmd = cmdBucket->getCommandData<SetMatrixUniformCommand>(setMatrixCmdHandle);

    cmd->matrixBuffer = SystemBuffer(matrixData, 0);
    cmd->uniformLocation = shaderUniform;
    cmd->transpose = false;
    cmd->numMatrices = numMatrices;
    return setMatrixCmdHandle;
}
