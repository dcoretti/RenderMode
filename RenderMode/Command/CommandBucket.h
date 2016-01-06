#pragma once

#pragma once
#include "../Memory/LinearAllocator.h"
#include "../Memory/PackedArray.h"

#include "Command.h"

struct CommandKey {
    int tmp;
};

class CommandBucket {
public:
    CommandBucket(LinearAllocator *commandPool, PackedArray<CommandKey> *keyPool);

    // create a blank command of type CommandType to be filled in externally.
    // allocates memory for the base command and the specific CommandType data, sets the dispatch field, and returns the blank data object to be filled
    template <typename CommandDataType> CommandDataType* createCommand(CommandKey key, size_t additionalMemory = 0);

    void clear();
private:
    // TODO pool these..
    CommandKey *keys;	// sortable keys in same order as corresponding commands array
    Command **commands;	// sortable pointers to commands in commandPool (sorted by key)
    int count{ 0 };

    LinearAllocator *commandPool;	// holds commands and command data in a packed array
    PackedArray<CommandKey> *keyPool;
    size_t size;
};
