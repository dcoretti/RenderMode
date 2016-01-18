#pragma once
#include "../Memory/IndexedPool.h"
#include "../Memory/LinearAllocator.h"
#include "../Memory/PackedArray.h"
#include "../Command/CommandBucket.h"
#include "../Render/RenderQueue.h"

#include "Model.h"
#include "Mesh.h"
#include "Material.h"

#include "GPU/GeometryTypes.h"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <string>
#include <unordered_map>
#include <vector>

#include "../Util/ObjLoader.h"
#include "../Render/RenderContext.h"

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
    Handle loadModel(std::string fname, RenderContext &rendercontext, CommandBucket &cmdBucket);
    void clearTransientPools();
private:
    struct TextureInfo {
        Handle textureHandle;
        void * textureData; // system data prior to loading to GPU
    };


    struct TransientModelDataInfo {
        void *vertices; // assumed float*3 (glm::vec3)
        void *normals;  // assumed float*3 (glm::vec3)
        void *texCoords; // assumed float*2 (glm::vec2)
        void *indices;  // assumed int
        unsigned int elements;
    };
    /*
    struct TransientModelDataInfo {
        unsigned int elements;
        unsigned int elementSize;
        void * geometryData;    // size elements*elementSize
    };*/

    TransientModelDataInfo ModelManager::convertObjToInternal(Model &model, RenderContext &renderContext, ModelObject &obj);

    void populateMaterial(RenderContext & renderContext, Material *material, ObjMaterial &objMaterial);
    TextureInfo loadTexture(RenderContext & renderContext, const std::string &textureName);
    void submitModelLoad(CommandBucket &cmdBucket,
                         const  Model& model,
                         const TransientModelDataInfo& transientModelData);


    // Transient data pools priort to laod into GPU
    LinearAllocator *meshDataPool;  // temporary pool for holding loaded resources in system ram until offloaded to gpu
    LinearAllocator *textureDataPool;

    unordered_map<std::string, TextureInfo> texInfoByName;
};


struct IndexedCoord {
    IndexedCoord(glm::vec3 vertex, glm::vec2 texCoord, glm::vec3 normal) :vertex(vertex), texCoord(texCoord), normal(normal) {}

    glm::vec3 vertex;
    glm::vec2 texCoord;
    glm::vec3 normal;

    bool operator==(const IndexedCoord& other) const {
        return vertex == other.vertex && texCoord == other.texCoord && normal == other.normal;
    }
};

namespace std {
    template<>
    struct hash<IndexedCoord> {
        size_t operator()(IndexedCoord const& i) const {
            hash<glm::vec3> h;
            hash<glm::vec2> h2;
            size_t seed = 0;
            glm::detail::hash_combine(seed, h(i.vertex));
            glm::detail::hash_combine(seed, h(i.normal));
            glm::detail::hash_combine(seed, h2(i.texCoord));
            return seed;
        }
    };
}
