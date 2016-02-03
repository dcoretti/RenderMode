#include "CommandBucket.h"



CommandBucket::CommandBucket(size_t maxCommands, size_t poolSize) {
    keys = new CommandKey[maxCommands];
    commandHandles = new Handle[maxCommands];
    commandPool = new LinearAllocator(poolSize);
    commandIndex = new PoolIndex(maxCommands);
}

CommandBucket::~CommandBucket() {
    delete[] keys;
    delete[] commandHandles;
    delete commandPool;
    delete commandIndex;
}

void CommandBucket::clear() {
    commandPool->clear();
    count = 0;
}

Command* CommandBucket::getCommand(Handle commandHandle) {
    return static_cast<Command*>(commandIndex->get(commandHandle));
}


void CommandBucket::submit() {
    // sort
    // submit to render
}

size_t CommandBucket::getPoolSize() {
    return commandPool->curSize();
}

int CommandBucket::getNumCommands() {
    return count;
}