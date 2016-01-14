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
    CommandBucket(LinearAllocator *commandPool, size_t maxCommands);
    ~CommandBucket();

    // create a blank command of type CommandType to be filled in externally.
    // allocates memory for the base command and the specific CommandType data, sets the dispatch field, and returns the blank data object to be filled
    template <typename CommandDataType> Handle createCommand(CommandKey key);

    template <typename CommandDataType> CommandDataType* getCommandData(Handle handle);

    Command* getCommand(Handle commandHandle);

    void clear();
private:
    int count{ 0 };
    CommandKey* keys;
    Handle *commandHandles;


    PoolIndex *commandIndex;
    LinearAllocator *commandPool;	// holds commands and command data in a packed array

};
