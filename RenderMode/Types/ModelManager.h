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
#include "../Render/RenderQueue.h"
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

    Model * loadModelGeometry(std::string &fname, RenderContext &rendercontext);
    void clearTransientPools();

    void submitModelLoadCommands(Handle modelGeometryHandle, RenderQueue &renderQueue, RenderContext &renderContext);

private:
    struct TextureInfo {
        Handle textureHandle;
        void * textureData; // system data prior to loading to GPU
    };

    void populateMaterial(RenderContext & renderContext, Material *material, ObjMaterial &objMaterial);
    TextureInfo loadTexture(RenderContext & renderContext, const std::string &textureName);





    // Transient data pools priort to laod into GPU
    LinearAllocator *meshDataPool;  // temporary pool for holding loaded resources in system ram until offloaded to gpu
    LinearAllocator *textureDataPool;

    unordered_map<std::string, TextureInfo> texInfoByName;

    CommandBuilder *cmdBuilder;
};


struct FaceIndex {
    FaceIndex(FaceElement e):v(e.v),t(e.t),n(e.n) {}
    FaceIndex(unsigned int v, unsigned int t, unsigned int n) :v(v),t(t),n(n) {}
    bool operator==(const FaceIndex &other) const {
        return v == other.v && t == other.t && n == other.n;
    }
    unsigned int v, t, n;
};


namespace std {
    template<>
    struct hash<FaceIndex> {
        size_t operator()(FaceIndex const &i) const {
            std::hash<unsigned int> h;
            size_t hash = 1;
            hash = hash * 17 + h(i.v);
            hash = hash * 31 + h(i.t);
            hash = hash * 13 + h(i.n);
            return hash;
        }
    };
}
