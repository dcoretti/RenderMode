#pragma once
#include "CommandBucket.h"

#include "../Types/Application/Model.h"
#include "../Types/Application/TransientModelData.h"
// TODO include key generation

/*
    Command builder adapts application level objects (meshes, models, materials) to render engine commands.

    Handles building data for commands from transient data and filling out api level objects for future use.
    Example: 
        A model is a set of handles interpreted by the render layer.
        A model is loaded using transient data.
        Building a load model command will build out the data behind the handles in the model object and construct commands to 
        utilize that data for loading.

        Rendering a model assumes that the Model object has been initialized by a buildLoadModelCommand type of command and 
        can use the handles to fill out a command.

    Two levels of building are done here: high level transformation of application types to commands (eg loading meshes), and mid level transformation to commands (eg loading vertex arrays)
*/
class CommandBuilder {
public:
    CommandBuilder(CommandBucket &cmdBucket, RenderContext &renderContext) 
        : cmdBucket(&cmdBucket), renderContext(&renderContext) { }
    /*  Low level methods (underlying data-types and cmd definitions) */


    Handle buildInitializeAndSetVertexArrayCommand(Handle &vao); // vao creation

    // Create a general vertex buffer as part of a command sequence
    Handle buildLoadVertexArrayCommandWithParent(SystemBuffer systemBuffer,
        Handle geometryBuffer,
        GPU::ShaderAttributeBinding shaderBinding,
        GPU::GeometryBufferLayout bufferLayout,
        Handle parentCommand);

    Handle buildLoadIndexArrayCommandWithParent(SystemBuffer systemBuffer, Handle geometryBuffer, Handle parentCommand);
    Handle buildLoadTextureWithParent(SystemBuffer systemBuffer, GPU::TextureBufferLayout textureBufferLayout, Handle texBuffer, Handle parentCommand);

    /* Shader Commands */
    Handle buildSetShaderProgramCommand(GPU::ShaderProgram shaderProgram);
    Handle buildCreateShaderCommand(GPU::ShaderData vertexShader, GPU::ShaderData fragmentShader, Handle shaderProgram);
    Handle buildSetMatrixUniformCommand(int shaderUniform, float* matrixData, int numMatrices, Handle parent = Handle());
    Handle buildSetVec3UniformCommand(int shaderUniform, float x, float y, float z, Handle parent = Handle());
    Handle buildSetFloatUniformCommand(int shaderUniform, float *vals, int count, Handle parent = Handle());
    Handle buildUpdateUniformBufferCommand(GPU::UniformBufferObject &ubo, void * data, size_t bufferSize, size_t offset, Handle parent);
    Handle buildCreateUniformBufferCommand(Handle uboHandle, size_t bufferSize, void * data,  int bufferBlockBinding, Handle parent = Handle());

    /* Draw Commands */
    Handle buildDrawArraysCommand(GPU::VertexArrayObject &vao, GPU::DrawContext &context);
    Handle buildDrawArraysCommandWithParent(GPU::VertexArrayObject &vao, GPU::DrawContext &context, Handle parentCommand);
    Handle buildDrawIndexedCommand(GPU::VertexArrayObject &vao, GPU::DrawContext &indexContext);
private:
    CommandBucket *cmdBucket;
    RenderContext *renderContext;
};