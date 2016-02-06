#pragma once

#include "../Memory/IndexedPool.h"
#include "../Types/GPU/GeometryTypes.h"
#include "../Types/Application/Material.h"
#include "../Types/Application/Mesh.h"
#include "../Types/Application/Model.h"


// All pooled data the rendering engine has access to.
struct RenderContext {
    RenderContext(size_t geometryBufferPoolSize,
        size_t geometryBufferLayoutPoolSize,
        size_t texturePoolSize,
        size_t shaderProgramsPoolSize,
        size_t materialPoolSize,
        size_t meshPoolSize,
        size_t modelPoolSize);
    // TODO change all of these to be handed a pool to allocate from rather than a size constructor.

    // All Vertex, normal, texCoords allocated here.  TODO type handles to allow separate pools.
    // Right now this is the only way we can get the render engine to know where to put the id it generates
    IndexedPool<GPU::GeometryBuffer> geometryBufferPool;
    IndexedPool<GPU::GeometryBufferLayout> geometryBufferLayoutPool;
    IndexedPool<GPU::Texture> texturePool;
    IndexedPool<GPU::ShaderProgram> shaderProgramsPool; // overkill with overhead?

    IndexedPool<Material> materialPool;
    IndexedPool<Mesh> meshPool;
    IndexedPool<Model> modelPool;  // All active models in a given context
};
