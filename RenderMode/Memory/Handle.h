#pragma once

/*
    POD Handle for indexed pooling of data that can be copied at will without depending
    on the underlying data's location in a memory pool.
*/
struct Handle {
    // TODO use variable bit size of index/version depending on type?
    unsigned int index{ 0 };
    unsigned short version{ 0 };

    // Note: should only be called from a pool
    void resetVersion() { version = 0; }
};