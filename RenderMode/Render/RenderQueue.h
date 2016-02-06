#pragma once
#include "../Command/Command.h"
#include "../Command/CommandBucket.h"
#include "../Render/RenderContext.h"
/*
    Queue of commands submitted by command buckets.
    Actually a ring buffer but sure whatever.
*/
class RenderQueue {
public:
    static const size_t queueSize = 256;

    bool submit(Handle *cmds, CommandKey *keys, size_t size);
    int execute(CommandBucket *cmdBucket, RenderContext &context);
    bool isEmpty();
private:
    struct CommandSet {
        CommandSet(Handle *cmds, CommandKey *keys, size_t numCommands) :
            cmds(cmds), keys(keys), numCommands(numCommands) {}
        Handle *cmds;
        CommandKey *keys;
        size_t numCommands;
    };

    CommandSet * pop();


    CommandSet queue[queueSize];
    int head{ 0 };
    unsigned int tail{ 0 };
    bool empty{ true };
};
