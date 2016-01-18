#pragma once

#include "../Memory/Handle.h"
#include "Mesh.h"

struct Model {
    ~Model() { 
        // kind of ugly but do this until the data is pooled somewhere.
        if (meshes != nullptr) {
            delete[] meshes; 
        } 
    }   
    Handle bufferLayout;
    Handle vertices;
    Handle normals;
    Handle texCoords;
    Handle indices;

    // to allow contiguous storage of related meshes, knowing only the number of meshes is necessary. 
    // This way the model pool can allocate storage together for a given mesh
    // Model | Mesh0 | ... | Meshn
    int numMeshes; 

    Handle * meshes{ nullptr };
};