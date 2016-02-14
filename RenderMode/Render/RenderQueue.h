#pragma once
#include "../Command/Command.h"
#include "../Command/CommandBucket.h"
#include "../Render/RenderContext.h"
/*
    Queue of commands submitted by command buckets.
    Actually a ring buffer but sure whatever.
*/
struct CommandSet {
    CommandSet() = default;
    CommandSet(Handle *cmds, CommandKey *keys, size_t numCommands) :
        cmds(cmds), keys(keys), numCommands(numCommands) {}
    Handle *cmds{ nullptr };
    CommandKey *keys{ nullptr };
    size_t numCommands{ 0 };
};

class RenderQueue {
public:
    static const size_t defaultQueueSize = 256;

    RenderQueue(unsigned int maxCommands);
    RenderQueue();
    ~RenderQueue();


    bool submit(Handle *cmds, CommandKey *keys, size_t size);
    int execute(CommandBucket &cmdBucket, RenderContext &context);
    CommandSet * pop();

    bool isEmpty();
    unsigned int numCommands();
private:




    CommandSet *queue;
    int head{ 0 };
    unsigned int tail{ 0 };

    unsigned int curCommands{ 0 };
    unsigned int maxCommands;
};
