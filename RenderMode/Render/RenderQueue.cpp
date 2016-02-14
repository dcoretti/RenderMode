#include "RenderQueue.h"

RenderQueue::RenderQueue(): RenderQueue(defaultQueueSize) {
}

RenderQueue::RenderQueue(unsigned int maxCommands): maxCommands(maxCommands) {
    queue = new CommandSet[maxCommands];
    
}

RenderQueue::~RenderQueue() {
    delete[] queue;
}


bool RenderQueue::submit(Handle * cmds, CommandKey *keys, size_t size) {
    if (curCommands == maxCommands) {
        return false;
    }
    queue[tail] = CommandSet(cmds, keys, size);
    tail = (tail + 1) % maxCommands;

    curCommands++;
    return true;
}

unsigned int RenderQueue::numCommands() {
    return curCommands;
}


CommandSet * RenderQueue::pop() {
    if (curCommands == 0) {
        return nullptr;
    }

    CommandSet *cmdSet = &queue[head];
    head = (head + 1) % maxCommands;

    curCommands--;
    return cmdSet;
}

bool RenderQueue::isEmpty() {
    return curCommands == 0;
}

int RenderQueue::execute(CommandBucket &cmdBucket, RenderContext &context) {
    int commandsRun = 0;
    while (!isEmpty()) {
        CommandSet * cmdSet = pop();
        for (int i = 0; i < cmdSet->numCommands; i++) {
            Handle handle = cmdSet->cmds[i];
            Command * cmd = cmdBucket.getCommand(handle);
            while (cmd != nullptr) {
                cmd->dispatch(cmdBucket.getDataFromCommand(cmd), context);
                cmd = cmd->next;
                commandsRun++;
            }
        }
    }
    return commandsRun;
}