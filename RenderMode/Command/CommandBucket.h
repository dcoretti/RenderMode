#pragma once

#pragma once
#include "../Dispatch/RenderApiDispatch.h"
#include "Command.h"

template <typename KeyType>
class CommandBucket {
public:
    CommandBucket(size_t size);

    // create a blank command of type CommandType to be filled in externally.
    // allocates memory for the base command and the specific CommandType data, sets the dispatch field, and returns the blank data object to be filled
    template <typename CommandDataType> CommandDataType* createCommand(KeyType key, Command * next);

    void submitCommands(RenderDispatch &dispatch);
    void clear();
private:
    // Call a command's dispatch function.
    void dispatchCommand(Command * command, RenderDispatch & dispatch);

    KeyType *keys;	// sortable keys in same order as corresponding commands array
    Command **commands;	// sortable pointers to commands in commandPool (sorted by key)
    int count{ 0 };

    LinearAllocator commandPool;	// holds commands and command data in a packed array
    size_t size;
};


template <typename KeyType>
CommandBucket<KeyType>::CommandBucket(size_t size) : commandPool(size) {
    keys = new KeyType[size];
    commands = new Command *[size];
}

template <typename KeyType>
void CommandBucket<KeyType>::clear() {
    commandPool.clear();
    count = 0;
}


template <typename KeyType>
template <typename CommandDataType> CommandDataType* CommandBucket<KeyType>::createCommand(KeyType key, Command * next) {
    // Command data expected layout
    // | Command c| c data  | Command c2 | c2 data        |
    // command dispatch fn is running on &c + sizeof(Command) 

    Command * cmd = new (commandPool)Command;
    // TODO
    // Maybe using a pointer to the data in the command would be better?
    // This might lend to better support for threading given that individual allocations could be atomic.
    // right now both have to occur in a contiguous block meaning that either this pool is thread specific
    // or we need a mutex around these two allocations.
    CommandDataType *cmdData = new (commandPool)CommandDataType;
    cmd->next = next;
    cmd->dispatch = CommandData<CommandDataType>::dispatchFn;

    // Store data in bucket
    keys[count] = key;
    commands[count] = cmd;
    count++;
    return cmdData;
}

template <typename KeyType>
void CommandBucket<KeyType>::dispatchCommand(Command * command, RenderDispatch & dispatch) {
    // see comment in createCommand about using a data ptr in the command itself instead
    // of assuming the data is right after the command

    // dispatch fn pointer in command is fed the address of the data immediately following the command struct
    //that data is a known size by the dispatch function.
    command->dispatch(command + 1, renderDispatch);
}


template<typename KeyType>
void CommandBucket<KeyType>::submitCommands(RenderDispatch &dispatch) {
    for (int i = 0; i < count; i++) {
        dispatchCommand(commands[i], dispatch);
    }
}