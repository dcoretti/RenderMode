#include "RenderQueue.h"

RenderQueue::RenderQueue(): RenderQueue(defaultQueueSize) {
}

RenderQueue::RenderQueue(unsigned int maxCommands): maxCommands(maxCommands) {
    queue = new Handle[maxCommands];
    
}

RenderQueue::~RenderQueue() {
    delete[] queue;
}

bool RenderQueue::submit(Handle cmd, CommandKey key) {
    if (curCommands == maxCommands) {
        return false;
    }
    queue[tail] = cmd;
    tail = (tail + 1) % maxCommands;

    curCommands++;
    return true;
}

unsigned int RenderQueue::numCommands() {
    return curCommands;
}


Handle RenderQueue::pop() {
    Handle h = queue[head];
    head = (head + 1) % maxCommands;

    curCommands--;
    return h;
}

bool RenderQueue::isEmpty() {
    return curCommands == 0;
}

int RenderQueue::execute(CommandBucket &cmdBucket, RenderContext &context) {
    int commandsRun = 0;
    while (!isEmpty()) {
        Handle handle = pop();
        Command * cmd = cmdBucket.getCommand(handle);
        while (cmd != nullptr) {
            cmd->dispatch(cmdBucket.getDataFromCommand(cmd), context);
            cmd = cmd->next;
            commandsRun++;
        }
    }
    return commandsRun;
}