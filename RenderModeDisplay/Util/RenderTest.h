
#pragma once
#include <string>

#include "Types\ModelManager.h"
#include "Command\CommandBucket.h"
#include "Command\CommandBuilder.h"
#include "Render\RenderContext.h"
#include "Render\RenderQueue.h"

#include <iostream>
using std::cout;
using std::endl;

//std::string squareObjFile =
//"v 0 0 0\n"
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
//
//
//float v[] = {
//    0, 0, 0,
//    1, 0, 0,
//    1, 1, 0,
//    0, 1, 0
//};
//
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


char vertexShader[] =
"#version 430 core\n"
"layout(location = 0) in vec4 pos;\n"
"void main() {\n"
"    gl_Position = pos;\n"
"}";

char fragmentShader[] =
"#version 430 core\n"
"out vec4 fragColor;\n"
"void main() {\n"
"    fragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
"}\n";

class RenderTest {
public:
    RenderTest() {
        renderContext = new RenderContext(1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024);
        cmdBucket = new CommandBucket(100, 1024 * 1024 * 5);
        cmdBuilder = new CommandBuilder(*cmdBucket, *renderContext);
        mgr = new ModelManager(1024, 1024, cmdBuilder);

        drawContext.indexOffset = 0;
        drawContext.numElements = 3;
    }

    ~RenderTest() {
        delete cmdBuilder;
        delete cmdBucket;
        delete mgr;
        delete renderContext;
    }



    void attemptLoadCommands() {
        cout << "starting test" << endl;
        int executed = 0;
        CommandKey key;

        //TODO
        // this order shouldn't be really needed.. VAO should be independent of the active program

        // Load and set shader
        shaderProgramHandle = renderContext->shaderProgramsPool.createObject();
        vertexShaderData.source = vertexShader;
        fragmentShaderData.source = fragmentShader;
        //vertexShaderData.type =
        Handle loadShaderCmd = cmdBuilder->buildCreateShaderCommand(vertexShaderData, fragmentShaderData, shaderProgramHandle);
        renderQueue.submit(&loadShaderCmd, &key, 1);

        // submit this separately because setshaderprogramcommand copies the shader program id so it needs to be submitted first.
        executed += renderQueue.execute(*cmdBucket, *renderContext);

        GPU::ShaderProgram *shader = renderContext->shaderProgramsPool.get(shaderProgramHandle);
        setShaderCmd = cmdBuilder->buildSetShaderProgramCommand(*shader);
        renderQueue.submit(&setShaderCmd, &key, 1);


        // load vertices
        Handle geometryBuffer = renderContext->geometryBufferPool.createObject();
        GPU::GeometryBufferLayout bufferLayout;

        vaoHandle = renderContext->vaoPool.createObject();
        Handle vaoParentCommand = cmdBuilder->buildInitializeAndSetVertexArrayCommand(vaoHandle);

        Handle loadCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(false,
            SystemBuffer((void*)&tri, sizeof(tri)),  // two triangles
            geometryBuffer,
            GPU::ShaderAttributeBinding::VERTICES,
            bufferLayout,
            vaoParentCommand);
        renderQueue.submit(&vaoParentCommand, &key, 1);

        executed += renderQueue.execute(*cmdBucket, *renderContext);

        cout << "commands executed: " << executed << endl;
        cout << "qsize: " << renderQueue.numCommands() << endl;

        drawCmd = cmdBuilder->buildDrawArraysCommand(*renderContext->vaoPool.get(vaoHandle), drawContext);
    }

    void draw() {
        CommandKey key;
        renderQueue.submit(&drawCmd, &key, 1);
        renderQueue.execute(*cmdBucket, *renderContext);
    }

    Handle drawCmd;
    GPU::DrawContext drawContext;

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

};
