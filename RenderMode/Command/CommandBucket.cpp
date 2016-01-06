#include "CommandBucket.h"



CommandBucket::CommandBucket(LinearAllocator *commandPool, PackedArray<CommandKey> *keyPool) :
    commandPool(commandPool), 
    keyPool(keyPool) {

}

void CommandBucket::clear() {
    commandPool->clear();
    keyPool->clear();
    count = 0;
}


template <typename CommandDataType> CommandDataType* CommandBucket::createCommand(CommandKey key, size_t additionalMemory) {
    // Command data expected layout
    // | Command c| c data  | Command c2 | c2 data        |
    // command dispatch fn is running on &c + sizeof(Command) 

    Command * cmd = new (commandPool)Command;
    if (additionalMemory > 0) {
        cmd->extraCmdData = (char *)commandPool->alloc(additionalMemory)
    }

    // Maybe using a pointer to the data in the command would be better?
    // This might lend to better support for threading given that individual allocations could be atomic.
    // right now both have to occur in a contiguous block meaning that either this pool is thread specific
    // or we need a mutex around these two allocations.
    CommandDataType *cmdData = new (commandPool)CommandDataType;
    cmd->dispatch = CommandData<CommandDataType>::dispatchFn;

    // Store data in bucket
    keys[count] = key;
    commands[count] = cmd;
    count++;
    return cmdData;
}
