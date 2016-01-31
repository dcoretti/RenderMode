#include "Command.h"

void dispatchDrawVertexBuffer(const void * data, const RenderApiDispatch & dispatch) {
    const DrawIndexedBufferCommand * cmdData = static_cast<const DrawIndexedBufferCommand *>(data);
    //dispatch.drawVertexBuffer(cmdData);
}

void dispatchSetShaderProgram(const void * data, const RenderApiDispatch & dispatch) {
    const SetShaderProgramCommand * cmdData = static_cast<const SetShaderProgramCommand *>(data);
    //dispatch.setShaders(cmdData);
}

void dispatchLoadArrayBuffer(const void * data, const RenderApiDispatch & dispatch) {
    const LoadArrayBufferCommand  *cmdData = static_cast<const LoadArrayBufferCommand *>(data);
}


void dispatchCreateShader(const void *data, const RenderApiDispatch & dispatch) {
    const CreateShaderCommand *cmdData = static_cast<const CreateShaderCommand *>(data);
}