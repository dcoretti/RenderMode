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
    // cannot be called once sorting is done.
    template <typename CommandDataType> Handle createCommand(CommandKey key);
    template <typename CommandDataType> Handle createCommand(Handle parentCommand);

    template <typename CommandDataType> CommandDataType* getCommandData(Handle handle);

    Command* getCommand(Handle commandHandle);

    void clear();
    void submit();

    size_t getPoolSize();
    int getNumSortableCommands();
    int getTotalCommands();
private:
    template <typename CommandDataType> Handle createCommandAndData();
    int sortableCount{ 0 }; // total sortable commands (number of commands in commandHandles, keys)
    int totalCount{ 0 };    // total count of commands including sub commands under the same sort key
    CommandKey* keys;
    Handle *commandHandles; // Handles for the "head" command in a command packet linked list.


    PoolIndex *commandIndex;
    LinearAllocator *commandPool;	// holds commands and command data in a packed array

};

template <typename CommandDataType> Handle CommandBucket::createCommandAndData() {
    // move this to a single allocation for both?
    Command * cmd = new (*commandPool)Command;
    cmd->dispatch = CommandData<CommandDataType>::dispatchFn;
    Handle commandHandle = commandIndex->createHandle((void *)cmd);
    CommandDataType *cmdData = new (*commandPool)CommandDataType;
    totalCount++;
    return commandHandle;
}


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
    Handle commandHandle = createCommandAndData<CommandDataType>();
    keys[sortableCount] = key;
    commandHandles[sortableCount] = commandHandle;
    sortableCount++;

    return commandHandle;
}

// Creates a command as normal, only appends to a list of commands.
// The command handle is not stored in commandHandles as it is not part of the sorting since it shares the 
// key of the parent command 
template <typename CommandDataType> Handle CommandBucket::createCommand(Handle parentCommand) {
    Command *cur = getCommand(parentCommand);
    while (cur->next != nullptr) {
        cur = cur->next;
    }
    
    Handle commandHandle = createCommandAndData<CommandDataType>();
    cur->next = getCommand(commandHandle);
    return commandHandle;
}
