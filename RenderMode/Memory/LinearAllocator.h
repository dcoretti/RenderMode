#pragma once

/*
* Linear allocator with no reuse for individual objects.
* ALL instances created using the placement syntax of this operator need to be manually destructed.
* delete blah where blah was allocated using the placement synax is undefined
*
*   TODO allow construction using a pointer to memory to allow allocator to be given memory when in use
*/
class LinearAllocator {
public:
    explicit LinearAllocator(size_t size);
    ~LinearAllocator();

    void * alloc(size_t size);
    void clear();

private:
    char * start; // using char * since std doesn't define pointer arithmetic on void *
    char * end;
    char * cur;
};

// Global new for use with a linear allocator.  Allocate the memory using a given instance of a linear allocator
// then return the location.  This location is used as input to the placement new 
// in the form new (operator new(size, allocator)) T(...);
// calling new (allocator) T(...) results in above call.
void * operator new(size_t size, LinearAllocator & allocator);

// we also need our own version of operator delete because any failed new from above needs to have a corresponding delete
void operator delete(void * ptr, LinearAllocator& allocator);


// TODO [] versions of new/delete