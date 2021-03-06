#pragma once
#include <type_traits>
/*
    POD Handle for indexed pooling of data that can be copied at will without depending
    on the underlying data's location in a memory pool.

    NOTE: Handles are meant to be used when referencing data owned by the core rendering engine.
    This allows data to be sorted and moved while keeping references pointing to the correct thing for systems submitting
    something like the VertexBuffer pointed to by a handle constant.

    Data used to organize render data (Models own a set of references to mesh pieces which are owned by the rendering system) 
    should NOT use Handle.

    TODO - add some sort of type or other safety to prevent handles from one pool being used on another accidentally.
    Right now that is owned by the IndexedPool, which returns only one type of data no matter the handle.
*/
static const unsigned int defaultHandleBaseVersion = 1;


struct Handle {
    // TODO use variable bit size of index/version depending on type?
    unsigned int index;
    unsigned short version;


    bool isValidHandle() {
        return version >= defaultHandleBaseVersion;
    }
};


static_assert(std::is_trivial<Handle>::value, "Handle must be trivial!");
static_assert(std::is_standard_layout<Handle>::value, "Handle must be a std layout!");

// TODO how to have a "null" handle?