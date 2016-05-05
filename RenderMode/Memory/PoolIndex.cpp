#include "PoolIndex.h"
#include "../Util/Common.h"

PoolIndex::PoolIndex(size_t indexSize):indices(indexSize) {
}

Handle PoolIndex::createHandle() {
    Handle handle;
    // Handle.index points to the index number in the sparse array.
    // That contains
    handle.index = indices.allocate();
    DBG_ASSERT(handle.index != -1, "Handle passed in has an uninitialized index value");

    InnerHandle * index = indices.get(handle.index);

    if (index->version < baseVersion) {
        index->version = baseVersion;
    }
    handle.version = index->version;
    return handle;
}

Handle PoolIndex::createHandle(void * data) {
    Handle handle = createHandle();
    setInnerIndexValue(handle, data);
    return handle;
}


bool PoolIndex::isValid(Handle handle) {
    InnerHandle *index = indices.get(handle.index);
    if (handle.version >= baseVersion && handle.version == index->version) {
        return true;
    }
    return false;
}

bool PoolIndex::isValid(Handle handle, InnerHandle *index) {
    if (index != nullptr &&
        handle.version >= baseVersion && 
        handle.version == index->version) {
        return true;
    }
    return false;
}


void PoolIndex::setInnerIndexValue(Handle handle, void * data) {
    if (isValid(handle)) {
        indices.get(handle.index)->data = data;
    }
}

void * PoolIndex::get(Handle handle) {
    InnerHandle * ih = indices.get(handle.index);
    return isValid(handle, ih) ? ih->data: nullptr;
}

void PoolIndex::clear() {
    baseVersion++;
    indices.clear();
}