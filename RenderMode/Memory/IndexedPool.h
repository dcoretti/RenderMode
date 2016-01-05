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
template <typename Data> // Handle should be of type Handle with index,version members
class IndexedPool {
public:
    IndexedPool(size_t size) : indices(size), dataPool(size) {}
    Handle createObject();
    void deleteObject(Handle handle);
    Data * get(Handle handle);

    int size() { return dataPool.getSize(); }
private:
    SparseArray<Handle> indices;
    PackedArray<Data> dataPool;
};


template <typename Data>
Handle IndexedPool<Data>::createObject() {
    Handle handle;
    handle.index = indices.allocate();

    Handle * index = indices.get(handle.index);
    handle.version = index->version++;
    index->version = handle.version;
    index->index = dataPool.allocate();

    return handle;
}

template <typename Data>
Data * IndexedPool<Data>::get(Handle handle) {
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


template <typename Data>
void IndexedPool<Data>::deleteObject(Handle handle) {
    Handle * index = indices.get(handle.index);
    if (index == nullptr) {
        return;
    }
    if (index->version != handle.version) {
        cout << "WARNING: (deleteObject) expected index version of" << handle.version << " but instead got: " << index->version
            << " for handleindex : " << handle.index << " and index: " << index->index << endl;
        return;
    }

    indices.remove(handle.index);
    int dataIndexToFix = dataPool.remove(index->index);
    if (dataIndexToFix != handle.index) {
        // Data at the dataIndexToFix was moved to replace data at handle.index to preserve packed array
        // update indices to point to new location
        Handle * toFix = indices.get(dataIndexToFix);
        toFix->index = handle.index;
    }
}