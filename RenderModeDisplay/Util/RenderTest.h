
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

std::string squareObjFile =
"v 0 0 0\n"
"v 1 0 0\n"
"v 1 1 0\n"
"v 0 1 0\n"
"vt 0 0\n"
"vt 1 0\n"
"vt 1 1\n"
"vt 0 1\n"
"vn 0 0 1\n"
"vn 0 0 1\n"
"vn 0 0 1\n"
"vn 0 0 1\n"
"f 1/1/1 2/2/2 3/3/3\n"
"f 1/1/1 3/3/3 4/4/4";


float v[] = {
    0, 0, 0,
    1, 0, 0,
    1, 1, 0,
    0, 1, 0
};

float vt[] = {
    0, 0,
    1, 0,
    1, 1,
    0, 1
};

float vn[] = {
    0, 0, 1,
    0, 0, 1,
    0, 0, 1,
    0, 0, 1
};

int i[] = {
    1, 1, 1,
    2, 2, 2,
    3, 3, 3,
    1, 1, 1,
    3, 3, 3,
    4, 4, 4
};


float onlyVertices[][3]{
    {-0.9f,-0.9f, 0.0f},
    {0.85f,-0.9f, 0.0f },
    {-0.9f, 0.85f, 0.0f },
    {0.9f,-0.85f, 0.0f },
    {0.9f,0.9f, 0.0f },
    {-0.85f,0.9f, 0.0f }
};


class RenderTest {
public:
    RenderTest() {
        renderContext = new RenderContext(1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024);
        cmdBucket = new CommandBucket(100, 1024 * 1024 * 5);
        cmdBuilder = new CommandBuilder(*cmdBucket, *renderContext);
        mgr = new ModelManager(1024, 1024, cmdBuilder);

        drawContext.indexOffset = 0;
        drawContext.numElements = 6;
    }

    ~RenderTest() {
        delete cmdBuilder;
        delete cmdBucket;
        delete mgr;
        delete renderContext;
    }



    void attemptLoadCommands() {

        cout << "command: " << sizeof(Command) << endl;
        cout << "cmd data load: " << sizeof(LoadArrayBufferCommand) << endl;
        cout << "starting test" << endl;
        Handle geometryBuffer = renderContext->geometryBufferPool.createObject();
        GPU::GeometryBufferLayout bufferLayout;

        vaoHandle = renderContext->vaoPool.createObject();
        Handle vaoParentCommand = cmdBuilder->buildInitializeAndSetVertexArrayCommand(vaoHandle);

        Handle loadCmd = cmdBuilder->buildLoadVertexArrayCommandWithParent(false,
            SystemBuffer((void*)&onlyVertices, sizeof(float) * 3 * 3 * 2),  // two triangles
            geometryBuffer,
            GPU::ShaderAttributeBinding::VERTICES,
            bufferLayout,
            vaoParentCommand);

        CommandKey key;
        renderQueue.submit(&vaoParentCommand, &key, 1);

        renderQueue.execute(*cmdBucket, *renderContext);
        cout << "commands executed!" << endl;
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

    Handle vaoHandle;
    ModelManager *mgr;
    CommandBucket *cmdBucket;
    CommandBuilder *cmdBuilder;
    RenderContext *renderContext;
    RenderQueue renderQueue;

};
