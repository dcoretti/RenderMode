#pragma once
#include "../CommonTypes.h"
/*
    Represents model data loaded from disk to be transferred to the GPU.
    
    1. Disk 
    2. TransientModelData (backed by data pool or other transient storage) 
    3. Model (Handles that point to GPU ids to be held by application layer)
*/

struct TransientModelData {
    TransientModelData(void * vertices, unsigned int verticesSize,
        void *normals, unsigned int normalsSize,
        void *texCoords, unsigned int texCoordsSize,
        void *indices, unsigned int indicesSize,
        unsigned int vertexCount):
        vertices(vertices, verticesSize),
        normals(normals, normalsSize),
        texCoords(texCoords, texCoordsSize),
        indices(indices, indicesSize),
        elements(elements) {}
    TransientModelData() = default;


    SystemBuffer vertices;
    SystemBuffer normals;
    SystemBuffer texCoords;
    SystemBuffer indices;

    unsigned int elements; // does this need to change for using indices vs just vertex array?


    unsigned int vertexComponents{ 3 };     // X,Y,Z by default
    unsigned int normalComponents{ 3 };     // X,Y,Z by default
    unsigned int texCoordComponents{ 2 };   // U,V by default

};

struct TransientTextureData {
    void *texData;
    int height;
    int width;

    // RGBA type info enum?
};


