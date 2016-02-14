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

    // rendering engine identifiers for the buffers
    // Only really used if updating the actual data, except for vao
    Handle vao;
    Handle vertices;
    Handle normals;
    Handle texCoords;
    Handle indices;

    // how is the data laid out in memory
    Handle vertexLayout;
    Handle normalsLayout;
    Handle texCoordsLayout;


    int numMeshes; 
    Handle * meshes{ nullptr };
};