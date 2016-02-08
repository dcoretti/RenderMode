
#pragma once
#include <string>

#include "Types\ModelManager.h"
#include "Command\CommandBucket.h"
#include "Command\CommandBuilder.h"
#include "Render\RenderContext.h"
#include "Render\RenderQueue.h"

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

class RenderTest {
public:
    RenderTest() {
        cmdBucket = new CommandBucket(100, 1024 * 1024 * 5);
        cmdBuilder = new CommandBuilder(cmdBucket);
        mgr = new ModelManager(1024, 1024, cmdBuilder);
        renderContext = new RenderContext(1024, 1024, 1024, 1024, 1024, 1024, 1024);
    }

    ~RenderTest() {
        delete cmdBuilder;
        delete cmdBucket;
        delete mgr;
        delete renderContext;
    }



    void attemptLoadCommands() {
        Handle geometryBuffer = renderContext->geometryBufferPool.createObject();
        
        Handle loadCmd = cmdBuilder->buildLoadVertexArrayCommand(geometryBuffer, false, &v, sizeof(float), 4);
        CommandKey key;
        renderQueue.submit(&loadCmd, &key, 1);

        renderQueue.execute(*cmdBucket, *renderContext);

    }
    ModelManager *mgr;
    CommandBucket *cmdBucket;
    CommandBuilder *cmdBuilder;
    RenderContext *renderContext;
    RenderQueue renderQueue;

};
