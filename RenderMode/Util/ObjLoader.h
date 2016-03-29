#pragma once 
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <glm/glm.hpp>
using std::unordered_map;
using std::vector;
using std::string;
using std::istringstream;
using std::istream;

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



/// TODO NEXT - replace inner fec of faces.  should be vector<vector<faces>> not face element 1 = 0-2, 2= 3-5 :((
struct FaceElement {
    FaceElement(int v, int t, int n) : v(v), t(t), n(n) {}
    FaceElement() = default;
    // 0 is an invalid index with obj format (1-based)
    int v{ 0 };
    int t{ 0 };
    int n{ 0 };
};

struct Group {
    Group() {}
    Group(string &groupName) : groupName(groupName) { }

    string groupName;

    // each face element will have 3 components regardless of what was specified.  Missing vt/vn will be denoted by 0 
    // **NOTE: all indices are 1 based in obj. 0 indicates a missing element which has been filled in
    // Each vector of face indices corresponds to a material state 1:1.  ex: materialStates[0] has a list of faces found in faces[0][0 - N]
    vector<vector<FaceElement> > faces;	// assumed v/n/t format so [0] is v, [1] is t, [2] is n, [3] is v1, etc.
                                // mtl? organize by starting index of faces that the mtl applies to.
                                // in free form mode mtl applies to vertex indices too..
    vector<MaterialState> materialStates;
};

struct ModelObject {
    string name;	// optional and no default.
    vector<glm::vec3> vertices;	// ordered vertices indexable from face elements (face index-1 = vertices array index)
    vector<glm::vec3> normals;
    vector<glm::vec2> texCoords;

    unordered_map<string, ObjMaterial> materials;  // materialname -> material
    unordered_map <string, Group> groups;
};

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
    static const string defaultGroupName;
    static const string defaultMaterialName;

    static ModelObject load(std::string &fname);
    static ModelObject load(istream &in);

private:    
    static void parseGroup(ModelObject &obj, State &curState, istringstream &line, int lineNum);

    static bool parseElement(State &curState, const string &token, istringstream &line, int lineNum);
    static bool parseVertexData(ModelObject &obj, const string &token, istringstream &line, int lineNum);

    static glm::vec3 parseVertex(istringstream & line, int lineNum);
    static void parseFace(State &curState, istringstream &line, int lineNum);

    static void loadMaterial(ModelObject &obj, istringstream &stream, int lineNum);
    static void parseMtl(ModelObject &obj, const string &fname);
    static void applyMtl(ModelObject &obj, State &curState, const string &materialName);

    static void printStats(ModelObject &obj);
};