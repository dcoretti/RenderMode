#pragma once

#include "../Memory/Handle.h"

/*
    Representation of a single mesh using a material.  Individual object may have several materials and are therefore 
    split by material

    TODO probably split mesh into two pieces, one for index/count and another for material
    since we can potentially render the same mesh with different materials
*/
struct Mesh {
    Handle material;
    unsigned int indexOffset;   // index of first vertex in buffer for this mesh.
    unsigned int numElements;
    // TODO use the vao itself? 
    Handle model;   // for access to vao.  
};
