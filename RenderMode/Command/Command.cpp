#include "Command.h"
#include "../Dispatch/RenderApiDispatch.h"

void dispatchDrawVertexBuffer(const void * data, RenderContext & context) {
    const DrawIndexedBufferCommand * cmdData = static_cast<const DrawIndexedBufferCommand *>(data);
}

void dispatchSetShaderProgram(const void * data, RenderContext & context) {
    const SetShaderProgramCommand * cmdData = static_cast<const SetShaderProgramCommand *>(data);
    //dispatch.setShaders(cmdData);
}

void dispatchLoadArrayBuffer(const void * data, RenderContext & context) {
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