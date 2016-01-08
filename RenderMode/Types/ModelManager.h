#pragma once
#include "../Memory/IndexedPool.h"
#include "../Memory/LinearAllocator.h"
#include "../Memory/PackedArray.h"

#include "Model.h"
#include "Mesh.h"
#include "Material.h"

#include "GPU/GeometryTypes.h"

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Util/ObjLoader.h"

using std::vector;
using std::unordered_map;
using std::string;

/*

    1. load resources into mesh data pool
    2. generate commands to load that data to the gpu when requested
    3. submit commands to render engine
    4. clear temp mesh data pool data (texture and mesh data)
    5. hold render metadata (GPU ids for component array buffers) in separate pool

    
*/
class ModelManager {
public:
    ModelManager(IndexedPool<Material>* materialPool, IndexedPool<Mesh> *meshPool, PackedArray<Model> *modelPool);

    Model * loadModel(std::string fname);
    void clearTransientPools();
private:
    struct TextureInfo {
        Handle textureHandle;
        void * textureData; // system data prior to loading to GPU
    };

    void ModelManager::convertObjToInternal(
        const vector<glm::vec3>& vertices,
        const vector<glm::vec3>& texCoords,
        const vector<glm::vec3>& normals,
        const unordered_map<string, Group>& groups,
        const unordered_map<string, ObjMaterial>& objMaterials);

    void populateMaterial(Material *material, ObjMaterial &objMaterial);
    TextureInfo loadTexture(const std::string &textureName);

   // void populateMaterial(Material * mat, const tinyobj::material_t &objMat);
    void ModelManager::populateMaterialTexture(Material * mat, const std::string textureName, Handle * handle);

    // Render engine data to be referenced by handles
    IndexedPool<Material> *materialPool;
    IndexedPool<Mesh> *meshPool;
    IndexedPool<GPU::Texture> *texturePool;


    // Asset level data to be referenced outside the rendering engine.
    PackedArray<Model> *modelPool;  // All active models in a given context

    // Transient data pools priort to laod into GPU
    // TODO have pool free memory entirely
    LinearAllocator *meshDataPool;  // temporary pool for holding loaded resources in system ram until offloaded to gpu
    LinearAllocator *textureDataPool;

    unordered_map<std::string, TextureInfo> texInfoByName;
};



struct IndexedCoord {
    IndexedCoord(glm::vec3 vertex, glm::vec3 texCoord, glm::vec3 normal) :vertex(vertex), texCoord(texCoord), normal(normal) {}

    glm::vec3 vertex;
    glm::vec2 texCoord;
    glm::vec3 normal;

    bool operator==(const IndexedCoord& other) const {
        return vertex == other.vertex && texCoord == other.texCoord && normal == other.normal;
    }
};


// General hash details
namespace std {
    template<>
    struct hash<glm::vec3> {
        size_t operator()(glm::vec3 const& v) const {
            size_t const h1(std::hash<float>()(v.x));
            size_t const h2(std::hash<float>()(v.y));
            size_t const h3(std::hash<float>()(v.z));
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    template<>
    struct hash<glm::vec2> {
        size_t operator()(glm::vec2 const& t) const {
            size_t const h1(std::hash<float>()(t[0]));
            size_t const h2(std::hash<float>()(t[1]));
            return h1 ^ (h2 << 1);
        }
    };

    template<>
    struct hash<IndexedCoord> {
        size_t operator()(IndexedCoord const& i) const {
            size_t const h1(std::hash<glm::vec3>()(i.vertex));
            size_t const h2(std::hash<glm::vec2>()(i.texCoord));
            size_t const h3(std::hash<glm::vec3>()(i.vertex));
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}
