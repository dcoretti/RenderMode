#pragma once
#include "../Command/Command.h"

/*
    Actually a ring buffer but sure whatever.
*/
class RenderQueue {
public:
    static const size_t queueSize = 1024;

    bool submit(Command * cmd);
    Command * pop();
    bool isEmpty();
private:
    Command *queue[queueSize];
    int head{ 0 };
    unsigned int tail{ 0 };
    bool empty{ true };
};
