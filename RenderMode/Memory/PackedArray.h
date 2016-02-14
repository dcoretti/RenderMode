#pragma once
#include <iostream>

using std::cout;
using std::end;

/*
    Packed array represents a contiguous chunk of memory holding only one type of object.  
    Removal of items in the middle of the array result in items from the end being copied to fill the gap, ensuring 
    that the array has zero fragmentation
*/
template<typename T>
class PackedArray {
public:
    PackedArray(size_t size);
    ~PackedArray() { delete[] dataArray; }
    int allocate();
    int remove(int index);
    T * get(int index);
    void clear();
    int getSize() { return end; }
private:
    T * dataArray;
    int end{ 0 };
    size_t size;
};

template<typename T>
PackedArray<T>::PackedArray(size_t size) : size(size) {
    // Note the () to zero-initialize - IMPORTANT for pod types like HANDLE
    dataArray = new T[size]();
}

// Get an index to the data in the array.
template<typename T>
int PackedArray<T>::allocate() {
    if (end == size) {
        return -1;
    }
    // (re)initialize data at that location
    T * data = new (&dataArray[end]) T;
    return end++;
}

// Returns the index of the element moved into slot index if done.
// returns index if nothing was moved
template<typename T>
int PackedArray<T>::remove(int index) {
    // copy data from back into this slot
    end--;
    if (index != end) {
        dataArray[index] = dataArray[end];
        return end;
    }
    return -1;
}

// Get data at a given index
template<typename T>
T * PackedArray<T>::get(int index) {
    if (index > end) {
        return nullptr;
    }
    return &dataArray[index];
}

template<typename T>
void PackedArray<T>::clear() {
    end = 0;
    size = 0;
}
