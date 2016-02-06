#pragma once

/*
    Represents model data loaded from disk to be transferred to the GPU.
    
    1. Disk 
    2. TransientModelData (backed by data pool or other transient storage) 
    3. Model (Handles that point to GPU ids to be held by application layer)
*/

struct TransientModelData {
    void *vertices; 
    void *normals; 
    void *texCoords;
    void *indices;  // assumed int
    unsigned int elements;

    unsigned int vertexElementSize{ sizeof(float) };
    unsigned int indexElementSize{ sizeof(int) };
    unsigned int vertexComponents{ 3 };
    unsigned int normalComponents{ 3 };
    unsigned int texCoordComponents{ 3 };

};