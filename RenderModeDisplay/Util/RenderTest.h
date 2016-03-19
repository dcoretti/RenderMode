
#pragma once
#include <string>

#include "Types\ModelManager.h"
#include "Command\CommandBucket.h"
#include "Command\CommandBuilder.h"
#include "Render\RenderContext.h"
#include "Render\RenderQueue.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
using std::cout;
using std::endl;

//std::string squareobjfile =
//"v - 0 0\n"
//"v 1 0 0\n"
//"v 1 1 0\n"
//"v 0 1 0\n"
//"vt 0 0\n"
//"vt 1 0\n"
//"vt 1 1\n"
//"vt 0 1\n"
//"vn 0 0 1\n"
//"vn 0 0 1\n"
//"vn 0 0 1\n"
//"vn 0 0 1\n"
//"f 1/1/1 2/2/2 3/3/3\n"
//"f 1/1/1 3/3/3 4/4/4";


float v[] = {
    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
};

unsigned int i[] = {
    0, 1, 2,
    0, 2, 3
};

//float vt[] = {
//    0, 0,
//    1, 0,
//    1, 1,
//    0, 1
//};
//
//float vn[] = {
//    0, 0, 1,
//    0, 0, 1,
//    0, 0, 1,
//    0, 0, 1
//};
//
//int i[] = {
//    1, 1, 1,
//    2, 2, 2,
//    3, 3, 3,
//    1, 1, 1,
//    3, 3, 3,
//    4, 4, 4
//};

//
//float onlyVerticesSq[]{
//    -0.9f,-0.9f, 1.0f,
//    0.85f,-0.9f, 1.0f,
//    -0.9f, 0.85f, 1.0f,
//    0.9f,-0.85f, 1.0f,
//    0.9f,0.9f, 1.0f,
//    -0.85f,0.9f, 1.0f 
//};

float tri[]{
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
};

float uv[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f
};

unsigned int triIndices[]{
    0, 1, 2
};

char vertexShader[] =
"#version 430 core\n"
"layout(location = 0) in vec4 pos;\n"
"uniform mat4 mvp;\n"
"void main() {\n"
"    gl_Position = mvp * pos;\n"
"}";

char fragmentShader[] =
"#version 430 core\n"
"out vec4 fragColor;\n"
"void main() {\n"
"    fragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
"}\n";


char vertexTex[] =
"#version 430 core\n"
"layout(location = 0) in vec4 pos;\n"
"layout(location = 1) in vec2 uv;\n"
"out vec2 outUv;"
"uniform mat4 mvp = mat4(1.0);\n"
"void main() {\n"
"    gl_Position = mvp * pos;\n"
"    outUv = uv;\n"

"}";

char fragmentTex[] =
"#version 430 core\n"
"out vec3 fragColor;\n"
"in vec2 uv;\n"
"uniform sampler2D texSampler;\n"
"void main() {\n"
"    fragColor = texture(texSampler, uv).rgb;"
"}";


int w = 4;
int h = 4;
// build a red RGB texture

unsigned char texData[] = {
    0xff, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0xff, 0x00, 0x00,  0xff, 0x00, 0x00,
    0x00, 0x00, 0x00,  0x00, 0x00, 0x00,  0xff, 0x00, 0x00,  0xff, 0x00, 0x00,
};

class RenderTest {
public:
    RenderTest() {
        renderContext = new RenderContext(1024, 1024, 1024, 1024, 1024, 1024, 1024);
        cmdBucket = new CommandBucket(100, 1024 * 1024 * 5);
        cmdBuilder = new CommandBuilder(*cmdBucket, *renderContext);
        mgr = new ModelManager(1024, 1024, cmdBuilder);

    }

    ~RenderTest() {
        delete cmdBuilder;
        delete cmdBucket;
        delete mgr;
        delete renderContext;
    }

    int setupShaders(char * vertex, char *frag) {
        int executed = 0;
        CommandKey key;
        // Load and set shader
        shaderProgramHandle = renderContext->shaderProgramsPool.createObject();
        vertexShaderData.source = vertex;
        fragmentShaderData.source = frag;
        //vertexShaderData.type =
        Handle loadShaderCmd = cmdBuilder->buildCreateShaderCommand(vertexShaderData, fragmentShaderData, shaderProgramHandle);
        renderQueue.submit(&loadShaderCmd, &key, 1);

        executed += renderQueue.execute(*cmdBucket, *renderContext);


        GPU::ShaderProgram *shader = renderContext->shaderProgramsPool.get(shaderProgramHandle);
        setShaderCmd = cmdBuilder->buildSetShaderProgramCommand(*shader);
        renderQueue.submit(&setShaderCmd, &key, 1);

        return executed;
    }

    void attemptLoadCommands(bool withCamera = false) {
        cout << "starting test" << endl;
        int executed = 0;
        CommandKey key;
        executed += setupShaders(vertexShader, fragmentShader);

        // load vertices
        Handle geometryBuffer = renderContext->geometryBufferPool.createObject();
        GPU::GeometryBufferLayout bufferLayout;

        vaoHandle = renderContext->vaoPool.createObject();
        Handle vaoParentCommand = cmdBuilder->buildInitializeAndSetVertexArrayCommand(vaoHandle);

        Handle loadCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(
            SystemBuffer((void*)&tri, sizeof(tri)),  // two triangles
            geometryBuffer,
            GPU::ShaderAttributeBinding::VERTICES,
            bufferLayout,
            vaoParentCommand);
        renderQueue.submit(&vaoParentCommand, &key, 1);

        executed += renderQueue.execute(*cmdBucket, *renderContext);

        cout << "commands executed: " << executed << endl;
        cout << "qsize: " << renderQueue.numCommands() << endl;

        GPU::DrawContext drawContext;
        drawContext.indexOffset = 0;
        drawContext.numElements = 3;
        if (withCamera) {
            glm::mat4 p = glm::perspective(glm::radians(90.0f), 640.0f / 480.0f, 0.1f, 100.0f);
            glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
            glm::mat4 v = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            //glm::mat4 v = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f));
            mvp = p * v * m;
            GPU::ShaderProgram *shader = renderContext->shaderProgramsPool.get(shaderProgramHandle);
            drawCmd = cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.mvp, glm::value_ptr(mvp), 1);
            cmdBuilder->buildDrawArraysCommandWithParent(*renderContext->vaoPool.get(vaoHandle), drawContext, drawCmd);
        }
        else {
            drawCmd = cmdBuilder->buildDrawArraysCommand(*renderContext->vaoPool.get(vaoHandle), drawContext);
        }
    }

    void loadIndexCommands() {
        cout << "starting test" << endl;
        int executed = 0;
        CommandKey key;
        executed += setupShaders(vertexShader, fragmentShader);

        // load vertices
        Handle geometryBufferHandle = renderContext->geometryBufferPool.createObject();
        Handle indexBufferHandle = renderContext->geometryBufferPool.createObject();
        GPU::GeometryBufferLayout bufferLayout;

        vaoHandle = renderContext->vaoPool.createObject();
        Handle vaoParentCommand = cmdBuilder->buildInitializeAndSetVertexArrayCommand(vaoHandle);
        Handle loadIndexCmd = cmdBuilder->buildLoadIndexArrayCommandWithParent(SystemBuffer((void*)&i, sizeof(i)),
            indexBufferHandle,
            vaoParentCommand);
        Handle loadVertexCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(
            SystemBuffer((void*)&v, sizeof(v)),  // two triangles
            geometryBufferHandle,
            GPU::ShaderAttributeBinding::VERTICES,
            bufferLayout,
            vaoParentCommand);
        renderQueue.submit(&vaoParentCommand, &key, 1);

        executed += renderQueue.execute(*cmdBucket, *renderContext);

        cout << "commands executed: " << executed << endl;
        cout << "qsize: " << renderQueue.numCommands() << endl;

        GPU::DrawContext drawContext;
        drawContext.indexOffset = 0;
        drawContext.numElements = 6;
        drawCmd = cmdBuilder->buildDrawIndexedCommand(*renderContext->vaoPool.get(vaoHandle), drawContext);
    }


    void texLoad(bool withCamera = false) {
        int executed = 0;
        CommandKey key;
        executed += setupShaders(vertexTex, fragmentTex);

        // load vertices
        Handle geometryBuffer = renderContext->geometryBufferPool.createObject();
        GPU::GeometryBufferLayout bufferLayout;

        vaoHandle = renderContext->vaoPool.createObject();
        Handle vaoParentCommand = cmdBuilder->buildInitializeAndSetVertexArrayCommand(vaoHandle);

        Handle loadCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(
            SystemBuffer((void*)&tri, sizeof(tri)),  
            geometryBuffer,
            GPU::ShaderAttributeBinding::VERTICES,
            bufferLayout,
            vaoParentCommand);

        Handle uvBuffer = renderContext->geometryBufferPool.createObject();
        GPU::GeometryBufferLayout uvLayout;
        uvLayout.numComponents = 2;
        Handle loadUvCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(
            SystemBuffer((void*)&uv, sizeof(uv)),
            uvBuffer,
            GPU::ShaderAttributeBinding::UV,
            uvLayout,
            vaoParentCommand);


        Handle texBuffer = renderContext->geometryBufferPool.createObject();
        GPU::TextureBufferLayout textureLayout;
        textureLayout.width = w;
        textureLayout.height = h;
        textureLayout.textureFormat = GPU::TextureFormat::RGB;
        Handle loadTexCmd = cmdBuilder->buildLoadTextureWithParent(SystemBuffer((void*)&texData, sizeof(texData)), 
            textureLayout, 
            texBuffer, 
            vaoParentCommand);


        renderQueue.submit(&vaoParentCommand, &key, 1);

        executed += renderQueue.execute(*cmdBucket, *renderContext);

        cout << "commands executed: " << executed << endl;
        cout << "qsize: " << renderQueue.numCommands() << endl;

        GPU::DrawContext drawContext;
        drawContext.indexOffset = 0;
        drawContext.numElements = 3;

        if (withCamera) {
            glm::mat4 p = glm::perspective(glm::radians(90.0f), 640.0f / 480.0f, 0.1f, 100.0f);
            glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
            glm::mat4 v = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            //glm::mat4 v = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f));
            mvp = p * v * m;
            GPU::ShaderProgram *shader = renderContext->shaderProgramsPool.get(shaderProgramHandle);
            drawCmd = cmdBuilder->buildSetMatrixUniformCommand(shader->uniformLocations.mvp, glm::value_ptr(mvp), 1);
            cmdBuilder->buildDrawArraysCommandWithParent(*renderContext->vaoPool.get(vaoHandle), drawContext, drawCmd);
        } else {
            drawCmd = cmdBuilder->buildDrawArraysCommand(*renderContext->vaoPool.get(vaoHandle), drawContext);
        }
    }


    void cameraTest() {



    }

    void draw() {
        CommandKey key;
        renderQueue.submit(&drawCmd, &key, 1);
        renderQueue.execute(*cmdBucket, *renderContext);
    }

    Handle drawCmd;

    // shader commands
    Handle shaderProgramHandle;
    GPU::ShaderData vertexShaderData;
    GPU::ShaderData fragmentShaderData;
    Handle setShaderCmd;


    Handle vaoHandle;
    ModelManager *mgr;
    CommandBucket *cmdBucket;
    CommandBuilder *cmdBuilder;
    RenderContext *renderContext;
    RenderQueue renderQueue;
    glm::mat4 mvp;
};
