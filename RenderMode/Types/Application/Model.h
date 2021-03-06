#pragma once

#include "../../Types/CommonTypes.h"
#include "../GPU/GeometryTypes.h"
#include "../../Memory/Handle.h"

/*
Representation of a single mesh using a material.  Individual object may have several materials and are therefore
split by material
*/
struct Mesh {
    Handle material;
    GPU::DrawContext drawContext;
};


// 2.  Used to draw a loaded model.
struct Model {
    Handle vao;
    Handle modelGeometryLoadDataHandle;
    size_t numMeshes;
    Mesh *meshes;
};


struct Geometry {
    Geometry() = default;
    Geometry(Handle bufferObject, GPU::GeometryBufferLayout bufferLayout, SystemBuffer bufferData) :
        bufferObject(bufferObject), bufferLayout(bufferLayout), bufferData(bufferData) {}
    Handle bufferObject;
    GPU::GeometryBufferLayout bufferLayout;
    SystemBuffer bufferData;
};

struct Indices {
    Indices() = default;
    Indices(Handle bufferObject, SystemBuffer bufferData) : bufferObject(bufferObject), bufferData(bufferData) {}
    Handle bufferObject;
    SystemBuffer bufferData;
};


// 1.  Load transient data to be transferred to GPU.
struct ModelGeometryLoadData {
    ModelGeometryLoadData() = default;
    ModelGeometryLoadData(Handle vao, Geometry vertices, Geometry normals, Geometry texCoords, Indices indices) :
        vao(vao), vertices(vertices), normals(normals), texCoords(texCoords), indices(indices) {}

    // To be filled out
    Handle vao;
    Geometry vertices;
    Geometry normals;
    Geometry texCoords;
    Indices indices;
};

