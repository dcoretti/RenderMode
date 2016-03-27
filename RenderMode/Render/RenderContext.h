#pragma once

#include "../Memory/IndexedPool.h"
#include "../Memory/LinearAllocator.h"
#include "../Types/GPU/GeometryTypes.h"
#include "../Types/GPU/ShaderTypes.h"
#include "../Types/Application/Material.h"
#include "../Types/Application/Model.h"


// All pooled data the rendering engine has access to.
struct RenderContext {
    RenderContext(size_t bufferObjectPoolSize,
        size_t shaderProgramsPoolSize,
        size_t materialPoolSize,
        size_t modelPoolSize,
        size_t modelGeometryPoolSize) :bufferObjectPool(IndexedPool<GPU::BufferObject>(bufferObjectPoolSize)),
        shaderProgramsPool(IndexedPool<GPU::ShaderProgram>(shaderProgramsPoolSize)),
        materialPool(IndexedPool<Material>(materialPoolSize)),
        modelPool(LinearAllocator(modelPoolSize)),
        modelGeometryPool(IndexedPool<ModelGeometryLoadData>(modelGeometryPoolSize))
    {
    }
    // TODO change all of these to be handed a pool to allocate from rather than a size constructor.

    // Right now this is the only way we can get the render engine to know where to put the id it generates
    IndexedPool<GPU::BufferObject> bufferObjectPool;
    IndexedPool<GPU::ShaderProgram> shaderProgramsPool; // overkill with overhead?
    IndexedPool<Material> materialPool;
    LinearAllocator modelPool;
    IndexedPool<ModelGeometryLoadData> modelGeometryPool;  // Transient geometry
};
