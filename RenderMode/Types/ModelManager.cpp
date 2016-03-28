#include "ModelManager.h"
#include <map>
using std::map;

ModelManager::ModelManager(size_t meshDataPoolSize, size_t textureDataPoolSize, CommandBuilder *cmdBuilder) 
    : cmdBuilder(cmdBuilder) {
    meshDataPool = new LinearAllocator(meshDataPoolSize);
    textureDataPool = new LinearAllocator(textureDataPoolSize);
}

ModelManager::~ModelManager() {
    delete meshDataPool;
    delete textureDataPool;
}

void  ModelManager::clearTransientPools() {
    // TODO give memory for pool back. Use a global pool for transient resource loading?
    meshDataPool->clear();
    textureDataPool->clear();
}

ModelManager::TextureInfo ModelManager::loadTexture(RenderContext & renderContext, const std::string &textureName) {
    if (texInfoByName.find(textureName) != texInfoByName.end()) {
        return texInfoByName[textureName];
    }

    // TODO: Do actual file loading into textureDataPool or maybe queue to be loaded at end of convertObjToInternal
    size_t textureSize = 1;
    void * texData = textureDataPool->alloc(textureSize);

    TextureInfo texInfo;
    texInfo.textureData = texData;
    texInfo.textureHandle = renderContext.bufferObjectPool.createObject();

    texInfoByName[textureName] = texInfo;


    return texInfo; // Handle to texture to be populated after load
}

/*
Model
    vnti  <- single index buffer.
    Mesh -
        material
        start of indices        <- note expectation of joined index buffer
        number of elements
*/
void ModelManager::populateMaterial(RenderContext & renderContext, Material *material, ObjMaterial &objMaterial) {
    material->ambient = objMaterial.ka;
    material->diffuse = objMaterial.kd;
    material->specular = objMaterial.ks;
    material->dissolve = objMaterial.dissolve;
    material->illum = objMaterial.illum;
    
    material->ambientTexture = loadTexture(renderContext, objMaterial.map_ka).textureHandle;
    material->diffuseTexture = loadTexture(renderContext, objMaterial.map_kd).textureHandle;
    material->specularTexture = loadTexture(renderContext, objMaterial.map_ks).textureHandle;
}

Handle ModelManager::loadModel(std::string fname, RenderContext &renderContext) {
    // TODO consider moving this to a constructor with rendercontext?
    ModelObject obj = ObjLoader::load(fname);

    // TODO these should be allocated on some pool
    map<string, Handle> materialHandles;
    size_t vSize = obj.vertices.size();
    size_t indexCount = 0;  // find a way to get full index count TODO <---------------------------------------------------


                            // Deduplicated coordinate data
                            // todo switch to placement new[]
    glm::vec3 *vertices = (glm::vec3*)meshDataPool->alloc(sizeof(glm::vec3) * vSize);
    glm::vec2 *texCoords = (glm::vec2*)meshDataPool->alloc(sizeof(glm::vec2) * vSize);
    glm::vec3 *normals = (glm::vec3*)meshDataPool->alloc(sizeof(glm::vec3) * vSize);
    unsigned int *indices = (unsigned int *)meshDataPool->alloc(sizeof(unsigned int) * indexCount);

    // convert all materials
    for (auto materialEntry : obj.materials) {
        Handle materialHandle = renderContext.materialPool.createObject();
        Material * material = renderContext.materialPool.get(materialHandle);

        populateMaterial(renderContext, material, materialEntry.second);
        materialHandles[materialEntry.second.name] = materialHandle;
    }

    // initialize containers for completed model/meshes

    /*
    for each face
    if v/t/n not in seenVtnCombinations
    add v/t/n actual values behind v/t/n index to deduplicated vertices,texCoords,normals  at position i
    add new IndexedCoord->i to seenVtnCombinations
    add i to new face list for mesh
    else
    add seenVtnCombinations[v/t/n] to face list for mesh

    NOTE each f v/t/n indexes into vertices texCoords, normals separately.  Deduplication by v/t/n tuple necessary
    */
    unsigned int curCoordIndex = 0;  // also total deduplicated v/t/n at the end of processing
    unsigned int curIndex = 0;       // index into indices array
    unordered_map<IndexedCoord, unsigned int> seenVtnCombinations; // Map of <V,T,N> -> index in deduplicated array

    if (obj.groups.size() > 1) {
        cout << "can only handle one obj group (one model) per obj file right now.  Got" << obj.groups.size() << endl;
        return Handle();
    }
    auto groupEntry = *obj.groups.begin();

    unsigned int curMaterial = 0;
    Model *model = new (renderContext.modelPool) Model;

    model->numMeshes = (int)groupEntry.second.faces.size();
    model->meshes = new (renderContext.modelPool) Mesh[model->numMeshes];

    for (vector<int> faceVector : groupEntry.second.faces) {
        Mesh * mesh = &model->meshes[curMaterial];

        mesh->drawContext.indexOffset = curIndex;   // starting index for all faces (v/t/n index) on this mesh

        for (int i = 0; i < faceVector.size(); i += 3) {
            // face elements are in form v/t/n with 1 as first index. 0 indicates missing element
            IndexedCoord indexedCoord(obj.vertices[faceVector[i] - 1],
                obj.texCoords[faceVector[i + 1] - 1],
                obj.normals[faceVector[i + 2] - 1]);

            if (seenVtnCombinations.find(indexedCoord) == seenVtnCombinations.end()) {
                vertices[curCoordIndex] = indexedCoord.vertex;
                texCoords[curCoordIndex] = indexedCoord.texCoord;
                normals[curCoordIndex] = indexedCoord.normal;
                indices[curIndex] = curIndex;
                curIndex++;
                curCoordIndex++;
            } else {
                indices[curIndex++] = seenVtnCombinations[indexedCoord];
            }
        }
        mesh->drawContext.numElements = curIndex;
        mesh->material = materialHandles[groupEntry.second.materialStates[curMaterial].materialName];
        curMaterial++;
    }


    Handle modelHandle = renderContext.modelGeometryPool.createObject();
    ModelGeometryLoadData *modelGeometry = new (renderContext.modelGeometryPool.get(modelHandle)) 
        ModelGeometryLoadData(renderContext.bufferObjectPool.createObject(),
        Geometry(renderContext.bufferObjectPool.createObject(),
            GPU::GeometryBufferLayout(),
            SystemBuffer(vertices, curCoordIndex * sizeof(glm::vec3))),
        Geometry(renderContext.bufferObjectPool.createObject(),
            GPU::GeometryBufferLayout(),
            SystemBuffer(normals, curCoordIndex * sizeof(glm::vec3))),
        Geometry(renderContext.bufferObjectPool.createObject(),
            GPU::GeometryBufferLayout(2, 0),
            SystemBuffer(texCoords, curCoordIndex * sizeof(glm::vec2))),
        Indices(renderContext.bufferObjectPool.createObject(),
            SystemBuffer(indices, curCoordIndex * sizeof(unsigned int))));


    return modelHandle;
}

void ModelManager::submitModelLoadCommands(ModelGeometryLoadData &data, RenderQueue &renderQueue) {
    Handle loadVao = cmdBuilder->buildInitializeAndSetVertexArrayCommand(data.vao);
    Handle loadVertices = cmdBuilder->buildLoadVertexArrayCommandWithParent(data.vertices.bufferData,
        data.vertices.bufferObject,
        GPU::ShaderAttributeBinding::VERTICES,
        data.vertices.bufferLayout,
        loadVao);
    Handle loadTextures = cmdBuilder->buildLoadVertexArrayCommandWithParent(data.texCoords.bufferData,
        data.texCoords.bufferObject,
        GPU::ShaderAttributeBinding::UV,
        data.texCoords.bufferLayout,
        loadVao);
    Handle loadNormals = cmdBuilder->buildLoadVertexArrayCommandWithParent(data.normals.bufferData,
        data.normals.bufferObject,
        GPU::ShaderAttributeBinding::NORMALS,
        data.normals.bufferLayout,
        loadVao);
    Handle loadIndices = cmdBuilder->buildLoadIndexArrayCommandWithParent(data.indices.bufferData, data.indices.bufferObject, loadVao);

    renderQueue.submit(loadVao, CommandKey());
}


void ModelManager::submitModelRenderCommands(Model &model, RenderContext &renderContext, RenderQueue &renderQueue) {


    for (int i = 0; i < model.numMeshes; i++) {
        Mesh &m = model.meshes[i];
        Handle meshCmd = cmdBuilder->buildDrawIndexedCommand(*renderContext.bufferObjectPool.get(model.vao), m.drawContext);
    }
    //cmdBuilder->buildDrawIndexedCommand(vaoid )
}
