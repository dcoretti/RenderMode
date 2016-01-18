#pragma once

#include "../Memory/IndexedPool.h"
#include "../Types/GPU/GeometryTypes.h"
#include "../Types/Material.h"

// All pooled data the rendering engine has access to.
struct RenderContext {
    // TODO change all of these to be handed a pool to allocate from rather than a size constructor.

    // All Vertex, normal, texCoords allocated here.  TODO type handles to allow separate pools.
    // Right now this is the only way we can get the render engine to know where to put the id it generates
    IndexedPool<GPU::GeometryBuffer> geometryBufferPool;


    IndexedPool<GPU::GeometryBufferLayout> geometryBufferLayoutPool;

    IndexedPool<GPU::Texture> texturePool;

    IndexedPool<Material> materialPool;
    IndexedPool<Mesh> meshPool;
    IndexedPool<Model> modelPool;  // All active models in a given context
};