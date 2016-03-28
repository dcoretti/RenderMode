#pragma once
#include "../Command/Command.h"
#include "../Command/CommandBucket.h"
#include "../Render/RenderContext.h"

class RenderQueue {
public:
    static const size_t defaultQueueSize = 256;

    RenderQueue(unsigned int maxCommands);
    RenderQueue();
    ~RenderQueue();

    bool submit(Handle cmd, CommandKey key);
    int execute(CommandBucket &cmdBucket, RenderContext &context);
    Handle pop();

    bool isEmpty();
    unsigned int numCommands();
private:
    Handle *queue;
    int head{ 0 };
    unsigned int tail{ 0 };

    unsigned int curCommands{ 0 };
    unsigned int maxCommands;
};
