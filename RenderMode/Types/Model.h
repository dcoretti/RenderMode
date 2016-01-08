#pragma once

#include "../Memory/Handle.h"
#include "Mesh.h"
struct Model {
    Handle bufferLayout;
    Handle vertexBuffere;
    Handle normal;
    Handle texCoords;
    Handle indices;

    // to allow contiguous storage of related meshes, knowing only the number of meshes is necessary. 
    // This way the model pool can allocate storage together for a given mesh
    // Model | Mesh0 | ... | Meshn
    int numMeshes; 
    Mesh * meshes;

    std::string name;
    // TODO - what can be used to referene the fact that the assets are loaded and ready to be processed?  Is that necessary?
};