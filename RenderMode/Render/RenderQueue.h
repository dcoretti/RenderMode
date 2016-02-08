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
    RenderQueue() = default;

    static const size_t queueSize = 256;

    bool submit(Handle *cmds, CommandKey *keys, size_t size);
    int execute(CommandBucket &cmdBucket, RenderContext &context);
    bool isEmpty();
private:
    struct CommandSet {
        CommandSet() = default;
        CommandSet(Handle *cmds, CommandKey *keys, size_t numCommands) :
            cmds(cmds), keys(keys), numCommands(numCommands) {}
        Handle *cmds{ nullptr };
        CommandKey *keys{ nullptr };
        size_t numCommands{ 0 };
    };

    CommandSet * pop();


    CommandSet queue[queueSize];
    int head{ 0 };
    unsigned int tail{ 0 };
    bool empty{ true };
};
