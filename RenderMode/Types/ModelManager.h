#pragma once
#include "../Memory/LinearAllocator.h"
#include "../Command/CommandBuilder.h"

#include "Application/Model.h"
#include "Application/Material.h"

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
    Model manager manages the creation of model and mesh objects.
    Handles initializing model, mesh objects to be used in command creation.  

    1. load resources into mesh data pool
    2. generate commands to load that data to the gpu when requested
    3. submit commands to render engine
    4. clear temp mesh data pool data (texture and mesh data)
    5. hold render metadata (GPU ids for component array buffers) in separate pool

    
*/
class ModelManager {
public:
    ModelManager(size_t meshDataPoolSize, size_t textureDataPoolSize, CommandBuilder *cmdBuilder);
    ~ModelManager();

    Handle loadModel(std::string fname, RenderContext &rendercontext);
    void clearTransientPools();
private:
    struct TextureInfo {
        Handle textureHandle;
        void * textureData; // system data prior to loading to GPU
    };

    void populateMaterial(RenderContext & renderContext, Material *material, ObjMaterial &objMaterial);
    TextureInfo loadTexture(RenderContext & renderContext, const std::string &textureName);



    Handle buildLoadModelCommand(ModelGeometryLoadData &data);
    void buildSetMaterialCommand(GPU::UniformBufferObject materialUniform, GPU::Uniform::Material &material);


    // Transient data pools priort to laod into GPU
    LinearAllocator *meshDataPool;  // temporary pool for holding loaded resources in system ram until offloaded to gpu
    LinearAllocator *textureDataPool;

    unordered_map<std::string, TextureInfo> texInfoByName;

    CommandBuilder *cmdBuilder;
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
