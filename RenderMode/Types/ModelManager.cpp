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
/*
Model * ModelManager::loadModel(std::string fname) {
    // TODO move this to the pool completely.
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    bool success = tinyobj::LoadObj(shapes, materials, err, fname.c_str());

    if (!err.empty() || !success) {
        std::cerr << err << std::endl;
        return nullptr;
    }

    Handle * materialHandles = new Handle[materials.size()];


    // Copy shapes to pool. TODO use custom obj loader to use memory pool directly.
    for (int i = 0; i < materials.size(); i++) {
        materialHandles[i] = materialPool->createObject();
        Material * mat = materialPool->get(materialHandles[i]);

        populateMaterial(mat, materials[i]);
    }

    // Convert shapes to models
    for (int i = 0; i < shapes.size(); i++) {
    }
    

    // Generate commands to submit to the render engine
}

ModelManager::TextureInfo ModelManager::loadTexture(const std::string &textureName) {
    if (texInfoByName.find(textureName) != texInfoByName.end()) {
        return texInfoByName[textureName];
    }

    // TODO: Do actual file loading into textureDataPool
    size_t textureSize = 1;
    void * texData = textureDataPool->alloc(textureSize);

    TextureInfo texInfo;
    texInfo.textureData = texData;
    texInfo.textureHandle = texturePool->createObject();

    texInfoByName[textureName] = texInfo;


    return texInfo; // Handle to texture to be populated after load
}

void ModelManager::populateMaterialTexture(Material * mat, const std::string textureName, Handle * handle) {
    TextureInfo texture;
    if (!textureName.empty()) {
        texture = loadTexture(textureName);
        *handle = texture.textureHandle;
    }
}

void ModelManager::populateMaterial(Material * mat, const tinyobj::material_t &objMat) {
    mat->ambient = glm::vec3(objMat.ambient[0], objMat.ambient[1], objMat.ambient[2]);
    mat->diffuse = glm::vec3(objMat.diffuse[0], objMat.diffuse[1], objMat.diffuse[2]);
    mat->specular = glm::vec3(objMat.specular[0], objMat.specular[1], objMat.specular[2]);

    mat->dissolve = objMat.dissolve;
    mat->illum = objMat.illum;

    populateMaterialTexture(mat, objMat.diffuse_texname, &mat->diffuseTextureHandle);
    populateMaterialTexture(mat, objMat.specular_texname, &mat->specularTextureHandle);
    populateMaterialTexture(mat, objMat.alpha_texname, &mat->ambientTextureHandle);
}

*/


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

// TODO

void ModelManager::populateMaterial(Material *material, ObjMaterial &objMaterial) {

    material->ambient = objMaterial.ka;
    material->diffuse = objMaterial.kd;
    material->specular = objMaterial.ks;
    material->dissolve = objMaterial.dissolve;
    material->illum = objMaterial.illum;

    
    material->ambientTextureHandle = loadTexture(objMaterial.map_ka).textureHandle;
    material->diffuseTextureHandle = loadTexture(objMaterial.map_kd).textureHandle;
    material->specularTextureHandle = loadTexture(objMaterial.map_ks).textureHandle;
}


void ModelManager::convertObjToInternal(
    const vector<glm::vec3>& objVertices, 
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
            mesh->materialHandle = materialHandles[groupEntry.second.materialStates[curMaterial].materialName];
            curMaterial++;
        }
    }
}