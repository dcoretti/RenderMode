#include "Command.h"

void dispatchDrawIndexedVertexBuffer(const void * data, const RenderApiDispatch & dispatch) {
    const DrawIndexedVertexBufferCommand * cmdData = static_cast<const DrawIndexedVertexBufferCommand *>(data);
    //dispatch.drawVertexBuffer(cmdData);
}

void dispatchSetShaderProgram(const void * data, const RenderApiDispatch & dispatch) {
    const SetShaderProgramCommand * cmdData = static_cast<const SetShaderProgramCommand *>(data);
    //dispatch.setShaders(cmdData);
}

void dispatchLoadArrayBuffer(const void * data, const RenderApiDispatch & dispatch) {
    const LoadArrayBufferCommand  *cmdData = static_cast<const LoadArrayBufferCommand *>(data);
}

void dispatchLoadIndexArrayBuffer(const void * data, const RenderApiDispatch & dispatch) {
    const LoadIndexArrayBufferCommand  *cmdData = static_cast<const LoadIndexArrayBufferCommand *>(data);
}
