#include "ModelManager.h"

ModelManager::ModelManager(IndexedPool<Material>* materialPool, IndexedPool<Mesh> *meshPool, PackedArray<Model> *modelPool) 
    : materialPool(materialPool), meshPool(meshPool), modelPool(modelPool) {

}

Model * ModelManager::loadModel(std::string fname) {
    return nullptr;
}