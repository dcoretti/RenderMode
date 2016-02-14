#include "Command.h"
#include "../Dispatch/RenderApiDispatch.h"


void dispatchSetShaderProgram(const void *data, RenderContext & context) {
    const SetShaderProgramCommand * cmdData = static_cast<const SetShaderProgramCommand *>(data);
    //dispatch.setShaders(cmdData);
}

void dispatchLoadArrayBuffer(const void *data, RenderContext & context) {
    const LoadArrayBufferCommand  *cmdData = static_cast<const LoadArrayBufferCommand *>(data);
    if (cmdData->isIndexArray) {
        RenderApiDispatch::loadArrayBuffer(context, cmdData);

    } else {
        RenderApiDispatch::loadIndexArrayBuffer(context, cmdData);
    }
}

void dispatchCreateShader(const void *data, RenderContext & context) {
    const CreateShaderCommand *cmdData = static_cast<const CreateShaderCommand *>(data);
}

void dispatchDrawVertexArray(const void *data, RenderContext &context) {
    const DrawVertexArrayCommand * cmdData = static_cast<const DrawVertexArrayCommand *>(data);
    RenderApiDispatch::drawVertexArray(context, cmdData);
}

void initializeAndSetVertexArray(const void *data, RenderContext &context) {
    const InitializeAndSetVertexArrayCommand * cmdData = static_cast<const InitializeAndSetVertexArrayCommand *>(data);
    RenderApiDispatch::initializeAndSetVertexArrayObject(context, cmdData);
}