#include "RenderApiDispatch.h"

#include <GL/glew.h>

void RenderApiDispatch::loadArrayBuffer(RenderContext &context, const LoadArrayBufferCommand *cmd) {
    GPU::GeometryBuffer *geometryBuffer = context.geometryBufferPool.get(cmd->geometryBuffer);
    glGenBuffers(1, &geometryBuffer->bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer->bufferId);
    glBufferData(GL_ARRAY_BUFFER, cmd->systemBufferSize, cmd->systemBuffer, GL_STATIC_DRAW);
}

void RenderApiDispatch::loadIndexArrayBuffer(RenderContext &context, const LoadArrayBufferCommand *cmd) {
    GPU::GeometryBuffer *geometryBuffer = context.geometryBufferPool.get(cmd->geometryBuffer);
    glGenBuffers(1, &geometryBuffer->bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometryBuffer->bufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cmd->systemBufferSize, cmd->systemBuffer, GL_STATIC_DRAW);
}
/*
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

void RenderApiDispatch::createShader(CreateShaderCommand *cmd) {
    // TODO - use proper pool to get shader id

    GLuint shader = glCreateShader(eShaderType);
    const char *strFileData = strShaderFile.c_str();
    glShaderSource(shader, 1, &strFileData, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLchar *strInfoLog = new GLchar[infoLogLength + 1];
    glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
    cout << "Compule failure for " << eShaderType << " shader: " << strInfoLog << endl;
    }
    return shader;

}
*/


