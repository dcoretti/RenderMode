#pragma once

#include "../Memory/PoolIndex.h"
#include "../Memory/LinearAllocator.h"

class IndexedLinearAllocator {
public:
    IndexedLinearAllocator(size_t size) {
        pool = new LinearAllocator(size);
        index = new PoolIndex(size);
    }
    IndexedLinearAllocator() {
        delete pool;
        delete index;
    }

    Handle allocate(size_t size) {
        void * data = pool->alloc(size);
        DBG_ASSERT(data != nullptr, "Could not allocate data");
        return index->createHandle(data);
    }

    void * get(Handle h) {
        return index->get(h);
    }
    
private:
    PoolIndex *index;
    LinearAllocator *pool;	// holds commands and command data in a packed array
};
