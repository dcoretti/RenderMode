#include "LinearAllocator.h"

LinearAllocator::LinearAllocator(size_t size) {
    // TODO move actual allocation to outside of this to allow platform specific memory allocation without new
    start = new char[size]();
    
    end = start + size;
    cur = start;
}

LinearAllocator::~LinearAllocator() {
    delete[] start;
}

void * LinearAllocator::alloc(size_t size) {
    if (size == 0 || cur + size > end) {
        return nullptr;
    }
    void * block = cur;
    cur += size;
    return block;
}

void LinearAllocator::clear() {
    cur = start;
    // TODO zero out option for debug?
}

size_t LinearAllocator::maxSize() {
    return end - start;
}

size_t LinearAllocator::curSize() {
    return (size_t)(cur - start);
}


void * operator new(size_t size, LinearAllocator & allocator){
    return allocator.alloc(size);
}

void * operator new[](size_t size, LinearAllocator & allocator){
    return allocator.alloc(size);
}

void operator delete(void * ptr, LinearAllocator& allocator) {
    // do nothing.  Linear allocators do not free individual allocations.  Only entire resets of the pool matter.
}
