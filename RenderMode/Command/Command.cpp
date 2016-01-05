#include "Command.h"

void dispatchDrawVertexBuffer(const void * data, const RenderApiDispatch & dispatch) {
    const DrawVertexBufferCommand * cmdData = static_cast<const DrawVertexBufferCommand *>(data);
    //dispatch.drawVertexBuffer(cmdData);
}

void dispatchSetShaders(const void * data, const RenderApiDispatch & dispatch) {
    const SetShadersCommand * cmdData = static_cast<const SetShadersCommand *>(data);
    //dispatch.setShaders(cmdData);
}