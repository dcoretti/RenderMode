#include "PoolIndex.h"

PoolIndex::PoolIndex(size_t indexSize):indices(indexSize) {
}

Handle PoolIndex::createHandle() {
    Handle handle;
    handle.index = indices.allocate();

    InnerHandle * index = indices.get(handle.index);
    handle.version = index->version++;
    index->version = handle.version;
    return handle;
}

Handle PoolIndex::createHandle(void * data) {
    Handle handle = createHandle();
    setInnerIndexValue(handle, data);
    return handle;
}


bool PoolIndex::isValid(Handle handle) {
    InnerHandle *index = indices.get(handle.index);
    if (handle.version == index->version) {
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
    return isValid(handle) ? indices.get(handle.index)->data: nullptr;
}
