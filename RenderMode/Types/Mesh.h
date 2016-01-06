#pragma once

#include "../Memory/Handle.h"

/*
    Representation of a single mesh using a material.  Individual object may have several materials and are therefore 
    split by material
*/
struct Mesh {
    Handle materialHandle;
    unsigned int indexOffset;
    unsigned int numElements;
};
