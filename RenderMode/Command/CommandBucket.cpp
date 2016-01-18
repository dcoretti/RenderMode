#include "CommandBucket.h"



CommandBucket::CommandBucket(LinearAllocator *commandPool, size_t maxCommands) : commandPool(commandPool) {
    keys = new CommandKey[maxCommands];
    commandHandles = new Handle[maxCommands];
}

CommandBucket::~CommandBucket() {
    delete[] keys;
    delete[] commandHandles;
}

void CommandBucket::clear() {
    commandPool->clear();
    count = 0;
}

Command* CommandBucket::getCommand(Handle commandHandle) {
    return static_cast<Command*>(commandIndex->get(commandHandle));
}

template <typename CommandDataType> CommandDataType* CommandBucket::getCommandData(Handle handle) {
    Command * cmd = static_cast<Command*>(commandIndex->get(handle));
    if (cmd == nullptr) {
        return nullptr;
    }
    return static_cast<CommandDataType *>(cmd + 1);
}


template <typename CommandDataType> Handle CommandBucket::createCommand(CommandKey key) {
    // Command data expected layout
    // | Command c| c data  | Command c2 | c2 data        |
    // command dispatch fn is running on &c + sizeof(Command) 
    Command * cmd = new (commandPool)Command;
    CommandDataType *cmdData = new (commandPool)CommandDataType;
    cmd->dispatch = CommandData<CommandDataType>::dispatchFn;
    Handle commandHandle = commandIndex->createHandle((void *)cmd);

    // Store data in bucket
    keys[count] = key;
    commandHandles[count] = commandHandle;
    count++;

    return commandHandle;
}


void CommandBucket::submit() {
    // sort
    // submit to render
}
