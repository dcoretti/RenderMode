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

    int numMeshes; 
    Handle * meshes{ nullptr };
};