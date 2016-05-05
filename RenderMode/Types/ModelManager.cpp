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

Model * ModelManager::loadModelGeometry(std::string &fname, RenderContext &renderContext) {
    // TODO consider moving this to a constructor with rendercontext?
    ModelObject obj = ObjLoader::load(fname);

    // TODO these should be allocated on some pool
    map<string, Handle> materialHandles;

    // convert all materials
    for (auto materialEntry : obj.materials) {
        Handle materialHandle = renderContext.materialPool.createObject();
        Material * material = renderContext.materialPool.get(materialHandle);

        populateMaterial(renderContext, material, materialEntry.second);
        materialHandles[materialEntry.second.name] = materialHandle;
    }


    unsigned int curMaterial = 0;

    Model *model = new (renderContext.modelPool) Model;
    model->numMeshes = obj.groups.size();
    model->meshes = new (renderContext.modelPool) Mesh[model->numMeshes];


    int numIndices = 0;
    for (const auto &g : obj.groups) {
        for (const auto &f : g.second.faces) {
            numIndices += (int)f.size();
        }
    }
    // TODO should this handle when we only have some of the components or is v/t/n always going to be specified?
    glm::vec3 *vertices = (glm::vec3*)meshDataPool->alloc(sizeof(glm::vec3) * numIndices);
    glm::vec2 *texCoords = (glm::vec2*)meshDataPool->alloc(sizeof(glm::vec2) * numIndices);
    glm::vec3 *normals = (glm::vec3*)meshDataPool->alloc(sizeof(glm::vec3) * numIndices);
    unsigned int *indices = (unsigned int *)meshDataPool->alloc(sizeof(unsigned int) * numIndices);
    int curIndex = 0;
    for (auto &group : obj.groups) {
        Mesh * mesh = &model->meshes[curMaterial];
        mesh->drawContext.indexOffset = curIndex;   
        for (vector<FaceElement> faceVector : group.second.faces) {
            for (auto faceElement : faceVector) {
                vertices[curIndex] = obj.vertices[faceElement.v - 1];
                normals[curIndex] = obj.normals[faceElement.n - 1];
                texCoords[curIndex] = obj.texCoords[faceElement.t - 1];
                indices[curIndex] = curIndex;
                curIndex++;
            }
        }
        mesh->drawContext.numElements = curIndex - mesh->drawContext.indexOffset;
    }

    model->modelGeometryLoadDataHandle = renderContext.modelGeometryPool.createObject();
    ModelGeometryLoadData *modelGeometry = new (renderContext.modelGeometryPool.get(model->modelGeometryLoadDataHandle))
        ModelGeometryLoadData(renderContext.bufferObjectPool.createObject(),
        Geometry(renderContext.bufferObjectPool.createObject(),
            GPU::GeometryBufferLayout(),
            SystemBuffer(vertices, numIndices * sizeof(glm::vec3))),
        Geometry(renderContext.bufferObjectPool.createObject(),
            GPU::GeometryBufferLayout(),
            SystemBuffer(normals, numIndices * sizeof(glm::vec3))),
        Geometry(renderContext.bufferObjectPool.createObject(),
            GPU::GeometryBufferLayout(2, 0),
            SystemBuffer(texCoords, numIndices * sizeof(glm::vec2))),
        Indices(renderContext.bufferObjectPool.createObject(),
            SystemBuffer(indices, numIndices * sizeof(unsigned int))));


    model->vao = modelGeometry->vao;
    return model;
}

void ModelManager::submitModelLoadCommands(Handle modelGeometryHandle, RenderQueue &renderQueue, RenderContext &renderContext) {
    ModelGeometryLoadData *data = renderContext.modelGeometryPool.get(modelGeometryHandle);



    Handle loadVao = cmdBuilder->buildInitializeAndSetVertexArrayCommand(data->vao);
    Handle loadVertices = cmdBuilder->buildLoadVertexArrayCommandWithParent(data->vertices.bufferData,
        data->vertices.bufferObject,
        GPU::ShaderAttributeBinding::VERTICES,
        data->vertices.bufferLayout,
        loadVao);
    Handle loadTextures = cmdBuilder->buildLoadVertexArrayCommandWithParent(data->texCoords.bufferData,
        data->texCoords.bufferObject,
        GPU::ShaderAttributeBinding::UV,
        data->texCoords.bufferLayout,
        loadVao);
    Handle loadNormals = cmdBuilder->buildLoadVertexArrayCommandWithParent(data->normals.bufferData,
        data->normals.bufferObject,
        GPU::ShaderAttributeBinding::NORMALS,
        data->normals.bufferLayout,
        loadVao);
    Handle loadIndices = cmdBuilder->buildLoadIndexArrayCommandWithParent(data->indices.bufferData, data->indices.bufferObject, loadVao);
    renderQueue.submit(loadVao, CommandKey());
}
