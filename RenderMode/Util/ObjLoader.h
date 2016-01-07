#pragma once 
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <glm/glm.hpp>
using std::unordered_map;
using std::vector;
using std::string;
using std::istringstream;

// TODO use GLM types for vertex data...  drop these structs!!
// how to sort?

struct Index {
    glm::vec3 vertex;
    glm::vec2 texCoord;
    glm::vec3 normal;

    bool operator==(const Index& other) const {
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
    struct hash<glm::vec2>  {
        size_t operator()(glm::vec2 const& t) const {
            size_t const h1(std::hash<float>()(t[0]));
            size_t const h2(std::hash<float>()(t[1]));
            return h1 ^ (h2 << 1);
        }
    };

    template<>
    struct hash<Index> {
        size_t operator()(Index const& i) const {
            size_t const h1(std::hash<glm::vec3>()(i.vertex));
            size_t const h2(std::hash<glm::vec2>()(i.texCoord));
            size_t const h3(std::hash<glm::vec3>()(i.vertex));
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

struct Object {
    string name;	// optional and no default.

                    // glm::vec3 Data
    vector<glm::vec3> vertices;	// ordered vertices indexable from face elements (face index-1 = vertices array index)
    vector<glm::vec3> normals;
    vector<glm::vec2> texCoords;


    //unordered_map<glm::vec3, int> vertex

};



struct ObjMaterial {
    ObjMaterial() {}
    ObjMaterial(string name, glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, float dissolve) : name(name), ka(ka), kd(kd), ks(ks), dissolve(dissolve) {}
    string name;
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    float dissolve;
    int illum; // illumination model #
    float Ns; // specular exponent

              // TODO map filenames as key to external storage.. load at some point
    string map_ka;
    string map_kd;
    string map_ks;
    string map_d;
    string map_bump;
    string map_disp;
    string map_decal;
};


struct MaterialState {
    int freeFormIndex;	//index of the first vertex in a groupings vertices array tied to this material.
    int faceIndex;		//index of first face in a groupings faceIndex tied to this material
    ObjMaterial * material;
    std::string materialName;
};

struct Group {
    Group() {}
    Group(string &groupName) : groupName(groupName) { }

    string groupName;

    // each face element will have 3 components regardless of what was specified.  Missing vt/vn will be denoted by 0 
    // **NOTE: all indices are 1 based in obj. 0 indicates a missing element which has been filled in
    // Each vector of face indices corresponds to a material state 1:1.  ex: materialStates[0] has a list of faces found in faces[0][0 - N]
    vector<vector<int> > faces;	// assumed v/n/t format so [0] is v, [1] is t, [2] is n, [3] is v1, etc.

                                // mtl? organize by starting index of faces that the mtl applies to.
                                // in free form mode mtl applies to vertex indices too..
    vector<MaterialState> materialStates;
};

/*
struct ObjMesh {
    string groupName;
    vector<int> indices;
    Material material;
};
*/

struct State {
    vector<Group *> activeGroups;	// Active groups specified by g gname1 ... gnameN
                                    // object id
                                    // group id
                                    // current material (default is a default white material) 
    ObjMaterial * curMaterial;	// Active material for a set of groups.
                            // current map usemap (off possible)
};


class ObjLoader {
public:
    ObjLoader();
    void load(std::string fname);


private:
    //void ObjLoader::deduplicateGroups(vector<glm::vec3> vertices, vector<glm::vec3> &normals, vector<glm::vec2> &texCoords, vector<ObjMesh> &unifiedGroups);
    
    
    void parseGroup(istringstream &line, int lineNum);

    bool parseElement(const string &token, istringstream &line, int lineNum);
    bool parseVertexData(const string &token, istringstream &line, int lineNum);

    glm::vec3 parseVertex(istringstream & line, int lineNum);
    void parseFace(istringstream &line, int lineNum);

    void loadMaterial(istringstream &stream, int lineNum);
    void parseMtl(const string &fname);
    void applyMtl(const string &materialName);

    void printStats();

    State curState;
    Object obj;

    unordered_map<string, ObjMaterial> materials;  // materialname -> material
    unordered_map <string, Group> groups;
};