#include "RenderContext.h"

RenderContext::RenderContext(size_t bufferObjectPoolSize,
    size_t geometryBufferLayoutPoolSize,
    size_t shaderProgramsPoolSize,
    size_t materialPoolSize,
    size_t meshPoolSize,
    size_t modelPoolSize,
    size_t vaoPoolSize)
    :bufferObjectPool(IndexedPool<GPU::BufferObject>(bufferObjectPoolSize)),
    shaderProgramsPool(IndexedPool<GPU::ShaderProgram>(shaderProgramsPoolSize)),
    materialPool(IndexedPool<Material>(materialPoolSize)),
    meshPool(IndexedPool<Mesh>(meshPoolSize)),
    modelGeometryPool(IndexedPool<ModelGeometryLoadData>(modelPoolSize)),
    vaoPool(IndexedPool<GPU::VertexArrayObject>(vaoPoolSize))
{
}