#include "RenderApiDispatch.h"

#include <GL/glew.h>

void RenderApiDispatch::loadArrayBuffer(RenderContext &context, const LoadArrayBufferCommand *cmd) {
    // assumes an already bound VAO for this context

    GPU::GeometryBuffer *geometryBuffer = context.geometryBufferPool.get(cmd->geometryBuffer);
    glGenBuffers(1, &geometryBuffer->bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, geometryBuffer->bufferId);
    glBufferData(GL_ARRAY_BUFFER, cmd->systemBuffer.sizeBytes, cmd->systemBuffer.data, GL_STATIC_DRAW);

    // Bind this array buffer to the shader layout and define the data format.
    glVertexAttribPointer(cmd->shaderBinding, 
        cmd->bufferLayout.numComponents, 
        GL_FLOAT,
        GL_FALSE, // not normalized
        cmd->bufferLayout.stride, (void*)0);
    glEnableVertexAttribArray(cmd->shaderBinding);
}

void RenderApiDispatch::loadIndexArrayBuffer(RenderContext &context, const LoadArrayBufferCommand *cmd) {
    // assumes an already bound VAO for this context
    GPU::GeometryBuffer *geometryBuffer = context.geometryBufferPool.get(cmd->geometryBuffer);
    glGenBuffers(1, &geometryBuffer->bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometryBuffer->bufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cmd->systemBuffer.sizeBytes, cmd->systemBuffer.data, GL_STATIC_DRAW);

}

//void RenderApiDispatch::enableVertexArray(RenderContext &context, const EnableVertexArrayCommand *cmd) {
//    // TODO no longer needed???
//}

void RenderApiDispatch::drawVertexArray(RenderContext &context, const DrawVertexArrayCommand *cmd) {
    glBindVertexArray(cmd->vao.vaoId);
    glDrawArrays(GL_TRIANGLES,cmd->drawContext.indexOffset, cmd->drawContext.numElements);
    glBindVertexArray(0);
}

void RenderApiDispatch::initializeAndSetVertexArrayObject(RenderContext &context, const InitializeAndSetVertexArrayCommand *cmd) {
    GPU::VertexArrayObject *vao = context.vaoPool.get(cmd->vertexArrayObject);
    glGenVertexArrays(1, &vao->vaoId);
    glBindVertexArray(vao->vaoId);
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


