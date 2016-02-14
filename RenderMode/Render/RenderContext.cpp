#include "RenderContext.h"

RenderContext::RenderContext(size_t geometryBufferPoolSize,
    size_t geometryBufferLayoutPoolSize,
    size_t texturePoolSize,
    size_t shaderProgramsPoolSize,
    size_t materialPoolSize,
    size_t meshPoolSize,
    size_t modelPoolSize,
    size_t vaoPoolSize)
    :geometryBufferPool(IndexedPool<GPU::GeometryBuffer>(geometryBufferPoolSize)),
    geometryBufferLayoutPool(IndexedPool<GPU::GeometryBufferLayout>(geometryBufferLayoutPoolSize)),
    texturePool(IndexedPool<GPU::Texture>(texturePoolSize)),
    shaderProgramsPool(IndexedPool<GPU::ShaderProgram>(shaderProgramsPoolSize)),
    materialPool(IndexedPool<Material>(materialPoolSize)),
    meshPool(IndexedPool<Mesh>(meshPoolSize)),
    modelPool(IndexedPool<Model>(modelPoolSize)),
    vaoPool(IndexedPool<GPU::VertexArrayObject>(vaoPoolSize))
{
}