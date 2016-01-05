#pragma once

template<typename T>
struct DataElement {
    T data;
    int next;
};


/*
    Sparse array represents a set of data with a embeded linked list of free elements. 
    Removal of an element appends the slot to the "free" list which is allocated from before slots on the end are used.
*/
template <typename T>
class SparseArray {
public:
    SparseArray(size_t size) : size(size) { dataArray = new DataElement<T>[size]; initVersion(); }
    int allocate();
    void remove(int index);
    T * get(int index);
private:
    void initVersion();

    DataElement<T> *dataArray;

    bool hasFree{ false };
    int free;
    int end{ 0 };
    size_t size;
};

template<typename T>
int SparseArray<T>::allocate() {
    if (end == size) {
        return -1;	// full :(
    }

    if (!hasFree) {
        return end++;
    } else {
        int dataLoc = free;
        int next = dataArray[free].next;
        if (next == -1) {
            hasFree = false;
        } else {
            free = next;
        }
        size++;

        return dataLoc;
    }
}

template <typename T>
void SparseArray<T>::remove(int index) {
    if (size == 0 || index >= end) {
        return;
    }

    int oldFree = free;
    free = index;
    dataArray[index].next = oldFree;
    size--;
}

template <typename T>
T * SparseArray<T>::get(int index) {
    return index < size ? &dataArray[index].data : nullptr;
}


template<typename T>
void SparseArray<T>::initVersion() {
    for (int i = 0; i < size; i++) {
        dataArray[i].data.version = 0;
    }
}
