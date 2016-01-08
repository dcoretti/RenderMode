#include "ModelManager.h"
#include <map>
using std::map;

ModelManager::ModelManager(IndexedPool<Material>* materialPool, IndexedPool<Mesh> *meshPool, PackedArray<Model> *modelPool) 
    : materialPool(materialPool), meshPool(meshPool), modelPool(modelPool) {

}

void  ModelManager::clearTransientPools() {
    // TODO give memory for pool back. Use a global pool for transient resource loading?
    meshDataPool->clear();
    textureDataPool->clear();
}

ModelManager::TextureInfo ModelManager::loadTexture(const std::string &textureName) {
    if (texInfoByName.find(textureName) != texInfoByName.end()) {
        return texInfoByName[textureName];
    }

    // TODO: Do actual file loading into textureDataPool or maybe queue to be loaded at end of convertObjToInternal
    size_t textureSize = 1;
    void * texData = textureDataPool->alloc(textureSize);

    TextureInfo texInfo;
    texInfo.textureData = texData;
    texInfo.textureHandle = texturePool->createObject();

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
void ModelManager::populateMaterial(Material *material, ObjMaterial &objMaterial) {
    material->ambient = objMaterial.ka;
    material->diffuse = objMaterial.kd;
    material->specular = objMaterial.ks;
    material->dissolve = objMaterial.dissolve;
    material->illum = objMaterial.illum;
    
    material->ambientTexture = loadTexture(objMaterial.map_ka).textureHandle;
    material->diffuseTexture = loadTexture(objMaterial.map_kd).textureHandle;
    material->specularTexture = loadTexture(objMaterial.map_ks).textureHandle;
}


void ModelManager::convertObjToInternal(const vector<glm::vec3>& objVertices,
                                        const vector<glm::vec3>& objTexCoords,
                                        const vector<glm::vec3>& objNormals,
                                        const unordered_map<string, Group>& groups,
                                        const unordered_map<string, ObjMaterial>& objMaterials) {

    map<string, Handle> materialHandles;


    size_t vSize = objVertices.size();
    size_t indexCount = 0;  // find a way to get full index count TODO <---------------------------------------------------


    // Deduplicated coordinate data
    // todo switch to placement new[]
    glm::vec3 *vertices = (glm::vec3*)meshDataPool->alloc(sizeof(glm::vec3) * vSize);
    glm::vec2 *texCoords = (glm::vec2*)meshDataPool->alloc(sizeof(glm::vec2) * vSize);
    glm::vec3 *normals = (glm::vec3*)meshDataPool->alloc(sizeof(glm::vec3) * vSize);
    unsigned int *indices = (unsigned int *)meshDataPool->alloc(sizeof(unsigned int) * indexCount);

    // convert all materials
    for (auto materialEntry : objMaterials) {
        Handle materialHandle = materialPool->createObject();
        Material * material = materialPool->get(materialHandle);

        populateMaterial(material, materialEntry.second);
        materialHandles[materialEntry.second.name] = materialHandle;
    }


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
    for (auto groupEntry : groups) {

        unsigned int curMaterial = 0;
        for (vector<int> faceVector : groupEntry.second.faces) {
            Mesh * mesh = new (meshDataPool) Mesh;
            mesh->indexOffset = curIndex;   // starting index for all faces (v/t/n index) on this mesh

            for (int i = 0; i < faceVector.size(); i += 3) {
                // face elements are in form v/t/n with 1 as first index. 0 indicates missing element
                IndexedCoord indexedCoord(objVertices[faceVector[i]-1], 
                    objTexCoords[faceVector[i + 1]-1], 
                    objNormals[faceVector[i + 2]-1]);

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
            mesh->numElements = curIndex;
            mesh->material = materialHandles[groupEntry.second.materialStates[curMaterial].materialName];
            curMaterial++;
        }
    }
}


void ModelManager::copyTransientModelDataToLoadCommand(LoadArrayBufferCommand * cmd, 
                                                       const TransientModelDataInfo &systemModelInfo, 
                                                       Handle geometryBuffer, 
                                                       bool isIndexArray) {

    cmd->elementSize = systemModelInfo.elementSize;
    cmd->geometryBuffer = geometryBuffer;
    cmd->systemBuffer = systemModelInfo.geometryData;
    cmd->systemBufferSize = systemModelInfo.elementSize * systemModelInfo.elements;
    cmd->isIndexArray = isIndexArray;
}

void ModelManager::submitModelLoad(CommandBucket &cmdBucket,
                                   const  Model& model, 
                                   const TransientModelDataInfo &vertexInfo, 
                                   const TransientModelDataInfo &normalInfo, 
                                   const TransientModelDataInfo &texInfo,
                                   const TransientModelDataInfo &indexInfo) {
    LoadArrayBufferCommand * loadVertexCmd = cmdBucket.createCommand<LoadArrayBufferCommand>(CommandKey(), 0);
    LoadArrayBufferCommand * loadNormalsCmd = cmdBucket.createCommand<LoadArrayBufferCommand>(CommandKey(), 0);
    LoadArrayBufferCommand * loadTexCoordsCmd = cmdBucket.createCommand<LoadArrayBufferCommand>(CommandKey(), 0);
    LoadArrayBufferCommand * loadIndicesCmd = cmdBucket.createCommand<LoadArrayBufferCommand>(CommandKey(), 0);

    copyTransientModelDataToLoadCommand(loadVertexCmd, vertexInfo, model.vertexBuffer, false);
    copyTransientModelDataToLoadCommand(loadNormalsCmd, vertexInfo, model.normal, false);
    copyTransientModelDataToLoadCommand(loadTexCoordsCmd, vertexInfo, model.texCoords, false);
    copyTransientModelDataToLoadCommand(loadIndicesCmd, vertexInfo, model.vertexBuffer, true);

    // TODO submit to render engine
}
