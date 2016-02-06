#include "RenderQueue.h"

bool RenderQueue::submit(Handle * cmds, CommandKey *keys, size_t size) {
    if (!empty && head == tail) {
        return false;
    }
    queue[tail] = CommandSet(cmds, keys, size);
    tail = tail + 1 % queueSize;
    empty = false;

    return true;
}

RenderQueue::CommandSet * RenderQueue::pop() {
    if (empty || head == tail) {
        return nullptr;
    }

    CommandSet *cmdSet = &queue[head];
    head = head+ 1 % queueSize;
    if (head == tail) {
        empty = true;
    }
    return cmdSet;
}

bool RenderQueue::isEmpty() {
    return empty;
}

int RenderQueue::execute(CommandBucket *cmdBucket, RenderContext &context) {
    int commandsRun = 0;
    while (!isEmpty()) {
        CommandSet * cmdSet = pop();
        for (int i = 0; i < cmdSet->numCommands; i++) {
            Handle handle = cmdSet->cmds[i];
            Command * cmd = cmdBucket->getCommand(handle);
            while (cmd != nullptr) {
                cmd->dispatch(cmdBucket->getDataFromCommand(cmd), context);
                cmd = cmd->next;
                commandsRun++;
            }
        }
    }
    return commandsRun;
}