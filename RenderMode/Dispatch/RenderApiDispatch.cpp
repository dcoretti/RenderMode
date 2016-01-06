#include "RenderApiDispatch.h"

#include <GL/glew.h>

void RenderApiDispatch::drawIndexVertexBuffer(DrawIndexedVertexBufferCommand *cmd) {
    //VertexBufferObject *vbo = meshPool.getData(cmd->handle);
    //glBindVertexArray(vbo->vaoId);
    glDrawArrays(GL_TRIANGLES, cmd->first, cmd->numVertices);
    glBindVertexArray(0);
}

void RenderApiDispatch::setShaderProgram(SetShaderProgramCommand *cmd) {
    glUseProgram(cmd->programId);
    // todo include aux memory to set uniforms
}

void RenderApiDispatch::loadArrayBuffer(LoadArrayBufferCommand *cmd) {
    unsigned int bufferId;

    glGenBuffers(1, &bufferId);
    //glGenVertexArrays(1, &vbo->vaoId);
    //glBindVertexArray(vbo->vaoId);
    //  glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    //glBufferData(GL_ARRAY_BUFFER, cmd->vertexCount, data, GL_STATIC_DRAW);
}

void RenderApiDispatch::loadIndexArrayBuffer(LoadIndexArrayBufferCommand *cmd) {

}
