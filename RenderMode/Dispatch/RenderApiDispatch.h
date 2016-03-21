#pragma once

#include "../Render/RenderContext.h"
#include "../Command/Command.h"

/*
    Dispatch a command's data to the graphics API specific implementation of that command.
    Currently only openGL but useful to split out between render apis
*/
namespace RenderApiDispatch {
    void initializeAndSetVertexArrayObject(RenderContext &context, const InitializeAndSetVertexArrayCommand *cmd);
    void loadArrayBuffer(RenderContext &context, const LoadArrayBufferCommand *cmd);
    void loadIndexArrayBuffer(RenderContext &context, const LoadIndexArrayBufferCommand *cmd);
    void loadTextureBuffer(RenderContext &context, const LoadTextureBufferCommand *cmd);
    // Draw
    //static void enableVertexArray(RenderContext &context, const EnableVertexArrayCommand *cmd);
    void drawVertexArray(RenderContext &context, const DrawVertexArrayCommand *cmd);
    void drawIndexedVertexArray(RenderContext &context, const DrawVertexArrayCommand *cmd);
    //void drawIndexVertexBuffer(DrawIndexedVertexBufferCommand *cmd);

    // Shaders
    void createShader(RenderContext &context, const CreateShaderCommand * cmd);
    void setShaderProgram(RenderContext &context, const SetShaderProgramCommand *cmd);
    void setMatrixUniform(RenderContext &context, const SetMatrixUniformCommand * cmd);

    void setFloatUniform(RenderContext &context, const SetFloatUniformCommand * cmd);
    void setVec3FloatUniform(RenderContext &context, const SetVec3UniformCommand * cmd);
    void initializeUniformBuffer(RenderContext &context, const InitializeUniformBufferCommand * cmd);
    void updateUniformBuffer(RenderContext &context, const UpdateUniformBufferCommand * cmd);
}
