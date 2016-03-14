#include "Command.h"
#include "../Dispatch/RenderApiDispatch.h"


void dispatchSetShaderProgram(const void *data, RenderContext & context) {
    const SetShaderProgramCommand * cmdData = static_cast<const SetShaderProgramCommand *>(data);
    RenderApiDispatch::setShaderProgram(context, cmdData);
}

void dispatchLoadArrayBuffer(const void *data, RenderContext & context) {
    const LoadArrayBufferCommand  *cmdData = static_cast<const LoadArrayBufferCommand *>(data);
    RenderApiDispatch::loadArrayBuffer(context, cmdData);
}

void dispatchCreateShader(const void *data, RenderContext & context) {
    const CreateShaderCommand *cmdData = static_cast<const CreateShaderCommand *>(data);
    RenderApiDispatch::createShader(context, cmdData);
}

void dispatchDrawVertexArray(const void *data, RenderContext &context) {
    const DrawVertexArrayCommand * cmdData = static_cast<const DrawVertexArrayCommand *>(data);
    if (cmdData->indexed) {
        RenderApiDispatch::drawIndexedVertexArray(context, cmdData);
    } else {
        RenderApiDispatch::drawVertexArray(context, cmdData);

    }
}

void initializeAndSetVertexArray(const void *data, RenderContext &context) {
    const InitializeAndSetVertexArrayCommand * cmdData = static_cast<const InitializeAndSetVertexArrayCommand *>(data);
    RenderApiDispatch::initializeAndSetVertexArrayObject(context, cmdData);
}

void dispatchLoadIndexArrayBuffer(const void * data, RenderContext & context) {
    const LoadIndexArrayBufferCommand * cmdData = static_cast<const LoadIndexArrayBufferCommand *>(data);
    RenderApiDispatch::loadIndexArrayBuffer(context, cmdData);
}
