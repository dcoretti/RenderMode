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

void RenderApiDispatch::loadIndexArrayBuffer(RenderContext &context, const LoadIndexArrayBufferCommand *cmd) {
    // assumes an already bound VAO for this context
    GPU::GeometryBuffer *geometryBuffer = context.geometryBufferPool.get(cmd->indexGeometryBuffer);
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

void RenderApiDispatch::drawIndexedVertexArray(RenderContext &context, const DrawVertexArrayCommand *cmd) {
    glBindVertexArray(cmd->vao.vaoId);
    glDrawElements(GL_TRIANGLES, cmd->drawContext.numElements, GL_UNSIGNED_INT, (void *) cmd->drawContext.indexOffset);
    glBindVertexArray(0);
}

void RenderApiDispatch::initializeAndSetVertexArrayObject(RenderContext &context, const InitializeAndSetVertexArrayCommand *cmd) {
    GPU::VertexArrayObject *vao = context.vaoPool.get(cmd->vertexArrayObject);
    glGenVertexArrays(1, &vao->vaoId);
    glBindVertexArray(vao->vaoId);
}


void checkShaderCompileStatus(unsigned int shaderId) {
    int status;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

        char *strInfoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog(shaderId, infoLogLength, NULL, strInfoLog);
        cout << "Compule failure for shader " << shaderId << " shader: " << strInfoLog << endl;
        delete[] strInfoLog;
    } else {
        cout << "Shader compilation successful." << endl;
    }
}

void checkShaderLinkStatus(unsigned int shaderProgram) {
    int status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, strInfoLog);

        cout << "Linker failure: " << strInfoLog << endl;
        delete[] strInfoLog;
    } else {
        cout << "Shader link successful." << endl;
    }
}



void RenderApiDispatch::createShader(RenderContext &context, const CreateShaderCommand * cmd) {
    GPU::ShaderProgram *shaderProgram = context.shaderProgramsPool.get(cmd->shaderProgram);

    shaderProgram->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shaderProgram->vertexShader, 1, &cmd->vertexShaderData.source, NULL);   // null for len assumes \0 terminated.
    glCompileShader(shaderProgram->vertexShader);
    checkShaderCompileStatus(shaderProgram->vertexShader);

    shaderProgram->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shaderProgram->fragmentShader, 1, &cmd->fragmentShaderData.source, NULL);   // null for len assumes \0 terminated.
    glCompileShader(shaderProgram->fragmentShader);
    checkShaderCompileStatus(shaderProgram->fragmentShader);

    shaderProgram->shaderProgramId = glCreateProgram();
    glAttachShader(shaderProgram->shaderProgramId, shaderProgram->vertexShader);
    glAttachShader(shaderProgram->shaderProgramId, shaderProgram->fragmentShader);
    glLinkProgram(shaderProgram->shaderProgramId);
    checkShaderLinkStatus(shaderProgram->shaderProgramId);

    glDetachShader(shaderProgram->shaderProgramId, shaderProgram->vertexShader);
    glDetachShader(shaderProgram->shaderProgramId, shaderProgram->fragmentShader);
    glDeleteShader(shaderProgram->vertexShader);
    glDeleteShader(shaderProgram->fragmentShader);
}


void RenderApiDispatch::setShaderProgram(RenderContext &context, const SetShaderProgramCommand *cmd) {
    glUseProgram(cmd->shaderProgram.shaderProgramId);
}