#pragma once

#include "../../Types/CommonTypes.h"
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


// 2.  Used to draw a loaded model.
struct Model {
    Handle vao;

    int numMeshes;
    Mesh *meshes;
};

// 1.  fill out this and construct load commands which will fill out the 
struct ModelGeometryLoadData {
    // To be filled out
    Handle vao;
    Handle vertices;
    Handle normals;
    Handle texCoords;
    Handle indices;

    // how is the data laid out in system buffers for loading
    GPU::GeometryBufferLayout vertexLayout;
    GPU::GeometryBufferLayout normalsLayout;
    GPU::GeometryBufferLayout texCoordsLayout;
    GPU::GeometryBufferLayout indicesLayout;


    SystemBuffer vertexData;
    SystemBuffer normalsData;
    SystemBuffer texCoordsData;
    SystemBuffer indicesData;
};