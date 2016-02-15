#pragma once

#include "../Render/RenderContext.h"
#include "../Command/Command.h"

/*
    Dispatch a command's data to the graphics API specific implementation of that command.
    Currently only openGL but useful to split out between render apis
*/
class RenderApiDispatch {
public:
    static void initializeAndSetVertexArrayObject(RenderContext &context, const InitializeAndSetVertexArrayCommand *cmd);
    static void loadArrayBuffer(RenderContext &context, const LoadArrayBufferCommand *cmd);
    static void loadIndexArrayBuffer(RenderContext &context, const LoadArrayBufferCommand *cmd);

    // Draw
    //static void enableVertexArray(RenderContext &context, const EnableVertexArrayCommand *cmd);
    static void drawVertexArray(RenderContext &context, const DrawVertexArrayCommand *cmd);
    //void drawIndexVertexBuffer(DrawIndexedVertexBufferCommand *cmd);

    // Shaders
    static void createShader(RenderContext &context, const CreateShaderCommand * cmd);
    static void setShaderProgram(RenderContext &context, const SetShaderProgramCommand *cmd);
};