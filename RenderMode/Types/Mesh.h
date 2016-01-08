#pragma once

#include "../Memory/Handle.h"

/*
    Representation of a single mesh using a material.  Individual object may have several materials and are therefore 
    split by material
*/
struct Mesh {
    Handle material;
    unsigned int indexOffset;   // index of first vertex in buffer for this mesh.
    unsigned int numElements;
};
