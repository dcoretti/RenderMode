#pragma once

template<typename T>
struct DataElement {
    T data;
    int next{ -1 };
};


/*
    Sparse array represents a set of data with a embeded linked list of free elements. 
    Removal of an element appends the slot to the "free" list which is allocated from before slots on the end are used.

    TODO - reclaim contiguous free spaces to avoid eventual linked list behavior
*/
template <typename T>
class SparseArray {
public:
    SparseArray(size_t maxSize) : maxSize(maxSize) { 
        // () to zero-initialize.  IMPORTANT for POD type storage to avoid initializing manually
        dataArray = new DataElement<T>[maxSize](); 
    }
    int allocate();
    void remove(int index);
    T * get(int index);
    unsigned int getNumElements() { return numElements; }
    void clear();
private:
    static const int NO_MORE_FREE = -1;
    DataElement<T> *dataArray;

    int free{ NO_MORE_FREE };
    int end{ 0 };
    size_t maxSize;

    unsigned int numElements{ 0 };
};

template<typename T>
int SparseArray<T>::allocate() {
    if (end == maxSize && numElements == maxSize) {
        return -1;	// full :(
    }

    int dataLoc;
    if (free == NO_MORE_FREE) {
        dataLoc = end++;
    } else {
        dataLoc = free;
        free = dataArray[free].next;
    }

    numElements++;
    return dataLoc;
}

template <typename T>
void SparseArray<T>::remove(int index) {
    if (numElements == 0 || index >= end) {
        return;
    }

    if (index == maxSize - 1) {
        end--;
    } else {
        int oldFree = free; // will be -1 in case of no free
        free = index;
        dataArray[index].next = oldFree;
    }

    numElements--;
}

template <typename T>
T * SparseArray<T>::get(int index) {
    return index < maxSize ? &dataArray[index].data : nullptr;
}

template<typename T>
void SparseArray<T>::clear() {
    free = NO_MORE_FREE;
    numElements = 0;
    end = 0;
}