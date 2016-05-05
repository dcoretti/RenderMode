#pragma once
#include "../Command/Command.h"
#include "../Command/CommandBucket.h"
#include "../Render/RenderContext.h"

class RenderQueue {
public:
    RenderQueue(CommandBucket *cmdBucket, RenderContext * renderContext, unsigned int queueSize);
    ~RenderQueue();


    bool submit(Handle cmd, CommandKey key);
    int execute();
    Handle pop();

    bool isEmpty();
    unsigned int numCommands();
private:
    Handle *queue;
    int head{ 0 };
    unsigned int tail{ 0 };

    unsigned int curCommands{ 0 };
    unsigned int maxCommands;
    CommandBucket *cmdBucket;
    RenderContext *renderContext;
};
