#include "RenderApiDispatch.h"

#include <GL/glew.h>

void RenderApiDispatch::loadArrayBuffer(RenderContext &context, const LoadArrayBufferCommand *cmd) {
    // assumes an already bound VAO for this context

    GPU::BufferObject *geometryBuffer = context.bufferObjectPool.get(cmd->geometryBuffer);
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
    GPU::BufferObject *geometryBuffer = context.bufferObjectPool.get(cmd->indexGeometryBuffer);
    glGenBuffers(1, &geometryBuffer->bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometryBuffer->bufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cmd->systemBuffer.sizeBytes, cmd->systemBuffer.data, GL_STATIC_DRAW);

}

void RenderApiDispatch::loadTextureBuffer(RenderContext &context, const LoadTextureBufferCommand *cmd) {
    GPU::BufferObject *geometryBuffer = context.bufferObjectPool.get(cmd->textureBuffer);
    glGenTextures(1, &geometryBuffer->bufferId);
    glBindTexture(GL_TEXTURE_2D, geometryBuffer->bufferId);
    // TODO use internal texture format and convert to gl format or have some sort of static mapping
    glTexImage2D(GL_TEXTURE_2D, 
        0, 
        GL_RGB, 
        cmd->textureBufferLayout.width, 
        cmd->textureBufferLayout.height, 
        0, 
        GL_RGB, 
        GL_UNSIGNED_BYTE, 
        cmd->systemBuffer.data);

    // TODO figure out something a bit more in depth here.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}


//void RenderApiDispatch::enableVertexArray(RenderContext &context, const EnableVertexArrayCommand *cmd) {
//    // TODO no longer needed???
//}

void RenderApiDispatch::drawVertexArray(RenderContext &context, const DrawVertexArrayCommand *cmd) {
    glBindVertexArray(cmd->vao.bufferId);
    glDrawArrays(GL_TRIANGLES,cmd->drawContext.indexOffset, cmd->drawContext.numElements);
    glBindVertexArray(0);
}

void RenderApiDispatch::drawIndexedVertexArray(RenderContext &context, const DrawVertexArrayCommand *cmd) {
    glBindVertexArray(cmd->vao.bufferId);
    glDrawElements(GL_TRIANGLES, cmd->drawContext.numElements, GL_UNSIGNED_INT, (void *) 0);
    glBindVertexArray(0);
}

void RenderApiDispatch::initializeAndSetVertexArrayObject(RenderContext &context, const InitializeAndSetVertexArrayCommand *cmd) {
    GPU::VertexArrayObject *vao = context.bufferObjectPool.get(cmd->vertexArrayObject);
    glGenVertexArrays(1, &vao->bufferId);
    glBindVertexArray(vao->bufferId);
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

void parseError(const char * msg) {
    GLenum errCode;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        cout << msg << " " << glGetError() << endl;
    }
}
// TODO Make this a separate command that can include arbitrary uniforms and map to a specified value location
void populateShaderUniformLocations(GPU::ShaderProgram * shaderProgram) {
    // single uniforms
    parseError("mvp");
    shaderProgram->uniformLocations.mvp = glGetUniformLocation(shaderProgram->shaderProgramId, GPU::Uniform::mvp);
    parseError("mvp");
    shaderProgram->uniformLocations.mv = glGetUniformLocation(shaderProgram->shaderProgramId, GPU::Uniform::mv);
    parseError("mv");
    shaderProgram->uniformLocations.v = glGetUniformLocation(shaderProgram->shaderProgramId, GPU::Uniform::v);
    parseError("v");
    shaderProgram->uniformLocations.m = glGetUniformLocation(shaderProgram->shaderProgramId, GPU::Uniform::m);
    parseError("m");

    shaderProgram->uniformLocations.diffuseTexture = glGetUniformLocation(shaderProgram->shaderProgramId, GPU::Uniform::diffuseTexture);
    shaderProgram->uniformLocations.normalMapTexture = glGetUniformLocation(shaderProgram->shaderProgramId, GPU::Uniform::normalMapTexture);

    // shader uniform blocks
    shaderProgram->uniformLocations.materialUniformBlockIndex = glGetUniformBlockIndex(shaderProgram->shaderProgramId, 
        GPU::Uniform::materialBlockIndex);
    glUniformBlockBinding(shaderProgram->shaderProgramId, 
        shaderProgram->uniformLocations.materialUniformBlockIndex,
        GPU::Uniform::defaultMaterialUniformBlockBinding);

    shaderProgram->uniformLocations.lightSourceUniformBlockIndex = glGetUniformBlockIndex(shaderProgram->shaderProgramId, 
        GPU::Uniform::lightSourceBlockIndex);
    glUniformBlockBinding(shaderProgram->shaderProgramId, 
        shaderProgram->uniformLocations.lightSourceUniformBlockIndex,
        GPU::Uniform::defaultLightSourceUniformBlockBinding);
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

    populateShaderUniformLocations(shaderProgram);
}


void RenderApiDispatch::setShaderProgram(RenderContext &context, const SetShaderProgramCommand *cmd) {
    glUseProgram(cmd->shaderProgram.shaderProgramId);
}

void RenderApiDispatch::setMatrixUniform(RenderContext &context, const SetMatrixUniformCommand * cmd) {
    glUniformMatrix4fv(cmd->uniformLocation, cmd->numMatrices, cmd->transpose, cmd->matrix);
}

void RenderApiDispatch::setFloatUniform(RenderContext &context, const SetFloatUniformCommand * cmd) {
    glUniform1fv(cmd->uniformLocation, cmd->count, cmd->vals);
}

void RenderApiDispatch::setVec3FloatUniform(RenderContext &context, const SetVec3UniformCommand * cmd) {
    glUniform3f(cmd->uniformLocation, cmd->x, cmd->y, cmd->z);
}

void RenderApiDispatch::initializeUniformBuffer(RenderContext &context, const InitializeUniformBufferCommand * cmd) {
    GPU::UniformBufferObject *ubo = context.bufferObjectPool.get(cmd->uboHandle);
    glGenBuffers(1, &ubo->bufferId);
    glBindBufferBase(GL_UNIFORM_BUFFER, cmd->bufferBlockBinding, ubo->bufferId);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo->bufferId);
    glBufferData(GL_UNIFORM_BUFFER, cmd->bufferSize, cmd->data, GL_DYNAMIC_DRAW);
    // TODO fix this
    // for some reason null and this works, but null followed by subdata in a separate command does not...
    //glBufferSubData(GL_UNIFORM_BUFFER, 0, cmd->bufferSize, cmd->data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderApiDispatch::updateUniformBuffer(RenderContext &context, const UpdateUniformBufferCommand * cmd) {
    glBindBuffer(GL_UNIFORM_BUFFER, cmd->ubo.bufferId);
    // TODO should use glMapBuffer for frequent changes?
    glBufferSubData(GL_UNIFORM_BUFFER, cmd->offset, cmd->bufferSize, cmd->data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}