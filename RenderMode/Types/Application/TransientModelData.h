#pragma once

/*
    Represents model data loaded from disk to be transferred to the GPU.
    
    1. Disk 
    2. TransientModelData (backed by data pool or other transient storage) 
    3. Model (Handles that point to GPU ids to be held by application layer)
*/

struct TransientModelData {
    TransientModelData(void * vertices, void *normals, void *texCoords, void *indices, unsigned int elements):
        vertices(vertices), normals(normals), texCoords(texCoords), indices(indices), elements(elements) {}
    TransientModelData() = default;

    void *vertices; 
    void *normals; 
    void *texCoords;
    void *indices;  // assumed int
    unsigned int elements;

    unsigned int vertexElementSize{ sizeof(float) };
    unsigned int indexElementSize{ sizeof(int) };
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