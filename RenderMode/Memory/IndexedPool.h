#pragma once
#include "SparseArray.h"
#include "PackedArray.h"
#include "Handle.h"

#include <iostream>
using std::cout;
using std::endl;

/*
    Indexed pool stores data contiguously in memory while allowing external applications to retrieve that
    data via handles.  Handles allow a lightweight key to actual data in the system.  
    Handles also enable efficient tracking of handle usage through version counting to easily detect handles being used outside their
    intended lifetime.  A version of a handle held by a calling application not matching the stored handle version indicates a leak.

*/
template <typename Data> 
class IndexedPool {
public:
    IndexedPool(size_t size) : maxSize(size), indices(size), dataPool(size) {}
    Handle createObject();
    void deleteObject(Handle handle);
    Data * get(Handle handle);

    int getSize() { return dataPool.getSize(); }
    size_t getMaxSize() { return maxSize; }
    void clear();
private:
    SparseArray<Handle> indices;
    PackedArray<Data> dataPool;

    // Base version for all Handle.version values in indices to allow quick clearing.
    unsigned int baseVersion{ defaultHandleBaseVersion };
    size_t maxSize;
};


template <typename Data>
Handle IndexedPool<Data>::createObject() {
    Handle handle;
    handle.index = indices.allocate();

    Handle * index = indices.get(handle.index);
    if (index->version < baseVersion) {
        index->version = baseVersion;
    }
    handle.version = index->version;

    index->index = dataPool.allocate();

    return handle;
}

template <typename Data>
Data * IndexedPool<Data>::get(Handle handle) {
    if (handle.version < baseVersion) {
        cout << "WARNING: handle version lower than base version. Handle to data prior to full clear of pool." << endl;
        return nullptr;
    }
    // get index from pool, retrieve data from index.
    Handle *index = indices.get(handle.index);
    if (index == nullptr) {
        return nullptr;
    }
    if (index->version != handle.version) {
        cout << "WARNING: (get) expected index version of" << handle.version << " but instead got: " << index->version
            << " for handleindex : " << handle.index << " and index: " << index->index << endl;
        return nullptr;
    }
    return dataPool.get(index->index);
}

// Delete will increase the internally stored version such that future get() calls for a handle will fail
template <typename Data>
void IndexedPool<Data>::deleteObject(Handle handle) {
    Handle * dataIndex = indices.get(handle.index);
    if (dataIndex == nullptr) {
        return;
    }
    if (dataIndex->version != handle.version) {
        cout << "WARNING: (deleteObject) expected index version of" << handle.version << " but instead got: " << dataIndex->version
            << " for handleindex : " << handle.index << " and index: " << dataIndex->index << endl;
        return;
    }

    dataIndex->version++; // mark a deleted object as such to avoid further get() calls to fail for that index
    indices.remove(handle.index); // mark index as free
    int dataIndexToFix = dataPool.remove(dataIndex->index);

    // Up to one element in the packed array is moved as the result of a delete.  Fix the index for that element
    if (dataIndexToFix != -1 && dataIndexToFix != handle.index) {
        // Data at the dataIndexToFix was moved to replace data at handle.index to preserve packed array
        // update indices to point to new location
        Handle * toFix = indices.get(dataIndexToFix);
        toFix->index = handle.index;
    }
}

template <typename Data>
void IndexedPool<Data>::clear() {
    baseVersion++;
    dataPool.clear();
    indices.clear();
}