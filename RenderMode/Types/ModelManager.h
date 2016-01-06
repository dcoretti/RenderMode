#pragma once
#include "../Memory/IndexedPool.h"
#include "../Memory/LinearAllocator.h"
#include "../Memory/PackedArray.h"

#include "Model.h"
#include "Mesh.h"
#include "Material.h"

#include <string>


/*

    1. load resources into mesh data pool
    2. generate commands to load that data to the gpu when requested
    3. submit commands to render engine
    4. clear temp mesh data pool data
    5. hold render metadata (GPU ids for component array buffers) in separate pool

*/
class ModelManager {
public:
    ModelManager(IndexedPool<Material>* materialPool, IndexedPool<Mesh> *meshPool, PackedArray<Model> *modelPool);

    Model * loadModel(std::string fname);

private:
    // Render engine data to be referenced by handles
    IndexedPool<Material>* materialPool;
    IndexedPool<Mesh> *meshPool;

    // Asset level data to be referenced outside the rendering engine.
    PackedArray<Model> *modelPool;  // All active models in a given context
    LinearAllocator *meshDataPool;  // temporary pool for holding loaded resources in system ram until offloaded to gpu
};