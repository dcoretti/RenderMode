#include "CommandBuilder.h"

/*
    High-level transformation (model->commands)
*/  

//Handle CommandBuilder::buildDrawModelCommand(const Model & mesh) {
//    return Handle();
//}
//
//Handle CommandBuilder::buildLoadModelCommand(Model & model, TransientModelData &modelData) {
//    // fill out model layout information using transient model data
//    Handle initVaoCmd = buildInitializeAndSetVertexArrayCommand(model.vao);
//
//    GPU::GeometryBufferLayout *vertexLayout = renderContext->geometryBufferLayoutPool.get(model.vertexLayout);
//    vertexLayout->numComponents = modelData.vertexComponents;
//
//    Handle loadVertexHandle = buildLoadVertexArrayCommandWithParent(modelData.vertices,
//            model.vertices,
//            GPU::ShaderAttributeBinding::VERTICES,
//            *renderContext->geometryBufferLayoutPool.get(model.vertexLayout),
//            initVaoCmd);
//
//
//    // FIX
//    // Do i need to set bools on model to denote if optional data is set???? can I be sure the handles 
//    // are validated when drawing to ensure we don't draw the normals if they don't exist?
//    // doesn't matter- VAO contains all the info so i won't need to use the handles unless updating.
//    if (modelData.normals.data != nullptr) {
//        GPU::GeometryBufferLayout *normalsLayout = renderContext->geometryBufferLayoutPool.get(model.normalsLayout);
//        normalsLayout->numComponents = modelData.normalComponents;
//
//        Handle loadNormalsHandle = buildLoadVertexArrayCommandWithParent(modelData.normals,
//            model.normals,
//            GPU::ShaderAttributeBinding::NORMALS,
//            *renderContext->geometryBufferLayoutPool.get(model.normalsLayout),
//            initVaoCmd);
//    }
//    if (modelData.texCoords.data != nullptr) {
//        GPU::GeometryBufferLayout *texCoordsLayout = renderContext->geometryBufferLayoutPool.get(model.texCoordsLayout);
//        texCoordsLayout->numComponents = modelData.texCoordComponents;
//        Handle loadTexCoordsCommand = buildLoadVertexArrayCommandWithParent(
//            modelData.texCoords,
//            model.texCoords,
//            GPU::ShaderAttributeBinding::UV,
//            *renderContext->geometryBufferLayoutPool.get(model.texCoordsLayout),
//            initVaoCmd);
//    }
//    if (modelData.indices.data != nullptr) {
//        // TODO
//    }
//
//    return initVaoCmd;
//}

/*
  Mid-level transforamtion of data to commands (buffers and layouts -> commands)
*/
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

Handle CommandBuilder::buildLoadTextureCommand(SystemBuffer systemBuffer, GPU::TextureBufferLayout textureBufferLayout, Handle textureBuffer, Handle parentCommand) {
    Handle loadTextureCmdHandle;
    if (parentCommand.isValidHandle()) {
        loadTextureCmdHandle = cmdBucket->createCommand<LoadTextureBufferCommand>(parentCommand);
    } else {
        loadTextureCmdHandle = cmdBucket->createCommand<LoadTextureBufferCommand>(CommandKey());
    }
    LoadTextureBufferCommand *cmd = cmdBucket->getCommandData<LoadTextureBufferCommand>(loadTextureCmdHandle);

    cmd->systemBuffer = systemBuffer;
    cmd->textureBuffer = textureBuffer;
    cmd->textureBufferLayout = textureBufferLayout;
    return loadTextureCmdHandle;
}

Handle CommandBuilder::buildSetTexturecommand(GPU::TextureBufferObject textureBufferObj, unsigned int textureBufferSlot, int uniformLocation, Handle parentCommand) {
    Handle setTextureCmdHandle;
    if (parentCommand.isValidHandle()) {
        setTextureCmdHandle = cmdBucket->createCommand<SetTextureCommand>(parentCommand);
    } else {
        setTextureCmdHandle = cmdBucket->createCommand<SetTextureCommand>(CommandKey());
    }

    // TODO maybe commands like this should be wrapped in a shader management setup to go from a GPU::ShaderProgram-> load cmds.
    SetTextureCommand *cmd = cmdBucket->getCommandData<SetTextureCommand>(setTextureCmdHandle);
    cmd->textureBufferObj = textureBufferObj;
    cmd->textureBufferSlot = textureBufferSlot;
    cmd->uniformLocation = uniformLocation;
    return setTextureCmdHandle;
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


Handle CommandBuilder::buildDrawIndexedCommand(GPU::VertexArrayObject &vao, GPU::DrawContext &indexContext, Handle parentCommand) {
    Handle drawArrayCmdHandle;
    if (parentCommand.isValidHandle()) {
        drawArrayCmdHandle = cmdBucket->createCommand<DrawVertexArrayCommand>(parentCommand);
    } else {
        drawArrayCmdHandle = cmdBucket->createCommand<DrawVertexArrayCommand>(CommandKey());
    }

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

Handle CommandBuilder::buildSetMatrixUniformCommand(int shaderUniform, float* matrixData, int numMatrices, Handle parent) {
    Handle setMatrixCmdHandle;
    if (parent.isValidHandle()) {
        setMatrixCmdHandle = cmdBucket->createCommand<SetMatrixUniformCommand>(parent);
    } else {
        setMatrixCmdHandle = cmdBucket->createCommand<SetMatrixUniformCommand>(CommandKey());
    }
    SetMatrixUniformCommand * cmd = cmdBucket->getCommandData<SetMatrixUniformCommand>(setMatrixCmdHandle);

    cmd->matrix = matrixData;
    cmd->uniformLocation = shaderUniform;
    cmd->transpose = false;
    cmd->numMatrices = numMatrices;
    return setMatrixCmdHandle;
}

Handle CommandBuilder::buildSetVec3UniformCommand(int shaderUniform, float x, float y, float z, Handle parent) {
    Handle vec3UniformHandle;
    if (parent.isValidHandle()) {
        vec3UniformHandle = cmdBucket->createCommand<SetVec3UniformCommand>(parent);
    } else {
        vec3UniformHandle = cmdBucket->createCommand<SetVec3UniformCommand>(CommandKey());
    }
    SetVec3UniformCommand *cmd = cmdBucket->getCommandData<SetVec3UniformCommand>(vec3UniformHandle);
    cmd->uniformLocation = shaderUniform;
    cmd->x = x;
    cmd->y = y;
    cmd->z = z;
    return vec3UniformHandle;
}

Handle CommandBuilder::buildSetFloatUniformCommand(int shaderUniform, float *vals, int count, Handle parent) {
    Handle floatUniformHandle;
    if (parent.isValidHandle()) {
        floatUniformHandle = cmdBucket->createCommand<SetFloatUniformCommand>(parent);
    } else {
        floatUniformHandle = cmdBucket->createCommand<SetFloatUniformCommand>(CommandKey());
    }
    SetFloatUniformCommand *cmd = cmdBucket->getCommandData<SetFloatUniformCommand>(floatUniformHandle);
    cmd->uniformLocation = shaderUniform;
    cmd->vals = vals;
    cmd->count = count;
    return floatUniformHandle;
}

Handle CommandBuilder::buildUpdateUniformBufferCommand(GPU::UniformBufferObject &ubo, void * data, size_t bufferSize, size_t offset, Handle parent) {
    Handle updateUniformHandle = cmdBucket->createCommand<UpdateUniformBufferCommand>(parent);
    UpdateUniformBufferCommand *cmd = cmdBucket->getCommandData<UpdateUniformBufferCommand>(updateUniformHandle);

    cmd->ubo = ubo;
    cmd->data = data;
    cmd->bufferSize = bufferSize;
    cmd->offset = offset;
    return updateUniformHandle;
}

Handle CommandBuilder::buildCreateUniformBufferCommand(Handle uboHandle, size_t bufferSize, void * data, int bufferBlockBinding, Handle parent) {

    Handle createUniformHandle;
    if (parent.isValidHandle()) {
        createUniformHandle = cmdBucket->createCommand<InitializeUniformBufferCommand>(parent);
    } else {
        createUniformHandle = cmdBucket->createCommand<InitializeUniformBufferCommand>(CommandKey());
    }

    InitializeUniformBufferCommand *cmd = cmdBucket->getCommandData<InitializeUniformBufferCommand>(createUniformHandle);

    cmd->uboHandle = uboHandle;
    cmd->data = data;
    cmd->bufferSize = bufferSize;
    cmd->bufferBlockBinding = bufferBlockBinding;

    return createUniformHandle;
}