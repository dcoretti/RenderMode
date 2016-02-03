#pragma once

#pragma once
#include "../Memory/PoolIndex.h"
#include "../Memory/LinearAllocator.h"
#include "../Memory/PackedArray.h"

#include "Command.h"

struct CommandKey {
    int tmp;
};

class CommandBucket {
public:
    CommandBucket(size_t maxCommands, size_t poolSize);
    ~CommandBucket();

    // create a blank command of type CommandType to be filled in externally.
    // allocates memory for the base command and the specific CommandType data, sets the dispatch field, and returns the blank data object to be filled
    template <typename CommandDataType> Handle createCommand(CommandKey key);

    template <typename CommandDataType> CommandDataType* getCommandData(Handle handle);

    Command* getCommand(Handle commandHandle);

    void clear();
    void submit();

    size_t getPoolSize();
    int getNumCommands();
private:
    int count{ 0 };
    CommandKey* keys;
    Handle *commandHandles; // Handles for the "head" command in a command packet linked list.


    PoolIndex *commandIndex;
    LinearAllocator *commandPool;	// holds commands and command data in a packed array

};


template <typename CommandDataType> CommandDataType* CommandBucket::getCommandData(Handle handle) {
    void * data =commandIndex->get(handle);
    Command *cmd = static_cast<Command*>(data);
    if (cmd == nullptr) {
        return nullptr;
    }
    // Command data occurs immediately after the command.
    void *cmdDataPtr = ((char *)data) + sizeof(Command);
    return static_cast<CommandDataType*>(cmdDataPtr);
}


template <typename CommandDataType> Handle CommandBucket::createCommand(CommandKey key) {
    // Command data expected layout
    // | Command c| c data  | Command c2 | c2 data        |
    // command dispatch fn is running on &c + sizeof(Command) 
    Command * cmd = new (*commandPool)Command;
    cmd->dispatch = CommandData<CommandDataType>::dispatchFn;

    Handle commandHandle = commandIndex->createHandle((void *)cmd);
    keys[count] = key;
    commandHandles[count] = commandHandle;
    count++;

    CommandDataType *cmdData = new (*commandPool)CommandDataType;

    return commandHandle;
}
