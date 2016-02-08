#pragma once

#include "SparseArray.h"
#include "Handle.h"

/*
    Index using handles to represent a mapping of handle to an index to some structure.
*/
class PoolIndex {
public:
    PoolIndex(size_t indexSize);
    Handle createHandle();
    Handle createHandle(void * data);
    bool isValid(Handle handle);

    void setInnerIndexValue(Handle handle, void * data);
    void * get(Handle handle);
    void clear();
private:
    struct InnerHandle {
        unsigned int version;
        void * data;
    };

    unsigned int baseVersion{ 0 };
    SparseArray<InnerHandle> indices;
};