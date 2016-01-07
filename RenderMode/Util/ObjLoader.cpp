#include "ObjLoader.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

using std::ifstream;
using std::getline;
using std::cout;
using std::endl;
using std::iterator;

ObjLoader::ObjLoader() {
    glm::vec3 w;
    w.x = 1.0f;
    w.y = 1.0f;
    w.z = 1.0f;
    ObjMaterial &white = materials["Default"];
    white.ka = w;
    white.kd = w;
    white.ks = w;
    white.dissolve = 1.0f;
    curState.curMaterial = &white;


    Group &defaultGroup = groups["default"];	// spec default group name
    defaultGroup.groupName = "default";

    curState.activeGroups.push_back(&defaultGroup);
}

void ObjLoader::load(std::string fname) {
    cout << "loading " << fname << endl;
    ifstream in(fname);
    if (!in.is_open()) {
        cout << "unable to load file " << fname << endl;
        return;
    }
    string line;
    int lineNum = 0;
    while (getline(in, line)) {
        if (line.length() == 0) {
            continue;
        }
        istringstream stream(line);
        string token;
        if (!(stream >> token)) {
            cout << "missing string token at beginning of line" << endl;
            return;
        }

        // Check top level organizational tags for major state changes
        if (token == "#") {
            // comment. skip
        }
        /* Grouping */
        else if (token == "o") {
            stream >> obj.name;
        } else if (token == "g") {
            parseGroup(stream, lineNum);
        }
        /* Render Attributes */
        else if (token == "mtllib") {
            loadMaterial(stream, lineNum);
        } else if (token == "usemtl") {
            string materialName;
            stream >> materialName;
            applyMtl(materialName);
        } else if (!parseElement(token, stream, lineNum) &&
            !parseVertexData(token, stream, lineNum)) {
            cout << "ERROR PARSING LINE " << lineNum << ": " << line << endl;
        }

        lineNum++;
    }
    in.close();
    printStats();
}

void ObjLoader::applyMtl(const string &materialName) {
    if (materials.find(materialName) == materials.end()) {
        cout << "material with name '" << materialName << "' doesn't exist.. initializing" << endl;
    }
    curState.curMaterial = &materials[materialName];
    ObjMaterial * material = curState.curMaterial;
    for (Group *g : curState.activeGroups) {
        MaterialState matState;
        // differentiate freeform vs face oriented reading?

        // Note that this index will be the index of the first face and freeform data that this material aplies to (not yet parsed)
        matState.faceIndex = (int)g->faces.size();
        matState.material = material;
        matState.materialName = materialName;
        g->materialStates.push_back(matState);
    }
}


bool ObjLoader::parseElement(const string &token, istringstream &line, int lineNum) {
    /* Elements */
    // TODO points, lines
    if (token == "f") {
        parseFace(line, lineNum);
        return true;
    }

}

bool ObjLoader::parseVertexData(const string &token, istringstream &line, int lineNum) {
    /* glm::vec3 Data */
    if (token == "v") {
        obj.vertices.push_back(parseVertex(line, lineNum));

    } else if (token == "vt") {
        glm::vec2 t;
        line >> t[0] >> t[1];
        obj.texCoords.push_back(t);
    } else if (token == "vn") {
        obj.normals.push_back(parseVertex(line, lineNum));

    } else if (token == "vp") {
        cout << "skipping unsupported vp token on line " << lineNum << endl;
    } else {
        return false;
    }
    return true;
}

// on newmtl spec
void ObjLoader::loadMaterial(istringstream &stream, int lineNum) {
    string matFile;
    if (!(stream >> matFile)) {
        cout << "missing material file name." << endl;
        return;
    }
    parseMtl(matFile);
}


// load/parse a MTL file
void ObjLoader::parseMtl(const string &fname) {
    cout << "Loading material file at '" << fname << "'" << endl;
    ifstream in(fname);
    if (!in.is_open()) {
        cout << "unable to load file " << fname << endl;
        return;
    }

    string line;
    int lineNum = 0;
    ObjMaterial *curMaterial;
    while (getline(in, line)) {
        if (line.length() == 0) {
            continue;
        }
        istringstream stream(line);
        string token;
        if (!(stream >> token)) {
            cout << "missing string token at beginning of line" << endl;
            return;
        }
        if (token == "#") {
            // comment. skip
        } else if (token == "newmtl") {
            string materialName;
            if (!(stream >> materialName)) {
                cout << "missing expected material name on line " << lineNum << endl;
                return;
            }
            if (materials.find(materialName) != materials.end()) {
                cout << "material with name '" << materialName << "' already exists.. replacing" << endl;
            }

            curMaterial = &materials[materialName];
            curMaterial->name = materialName;
        } else if (token == "Ka") {
            stream >> curMaterial->ka.x >> curMaterial->ka.y >> curMaterial->ka.z;
        } else if (token == "Kd") {
            stream >> curMaterial->kd.x >> curMaterial->kd.y >> curMaterial->kd.z;
        } else if (token == "Ks") {
            stream >> curMaterial->ks.x >> curMaterial->ks.y >> curMaterial->ks.z;
        } else if (token == "d" || token == "Tr") {
            stream >> curMaterial->dissolve;
        } else if (token == "map_Ka") {
            stream >> curMaterial->map_ka;
        } else if (token == "map_Kd") {
            stream >> curMaterial->map_kd;
        } else if (token == "map_Ks") {
            stream >> curMaterial->map_ks;
        } else if (token == "map_d") {
            stream >> curMaterial->map_d;
        } else if (token == "map_bump" || token == "bump") {
            stream >> curMaterial->map_bump;
        } else if (token == "disp") {
            // displacement map
            stream >> curMaterial->map_disp;
        } else if (token == "decal") {
            stream >> curMaterial->map_decal;
        } else if (token == "illum") {
            stream >> curMaterial->illum;
        } else if (token == "Ns") {
            stream >> curMaterial->Ns;
        } else {
            cout << "ERROR Invalid token for line " << lineNum << ": " << line << endl;
            return;
        }

        lineNum++;
    }
    in.close();
}

glm::vec3 ObjLoader::parseVertex(istringstream & line, int lineNum) {
    glm::vec3 v;
    // todo is color after ?
    if (!(line >> v.x >> v.y >> v.z)) {
        cout << "invalid vertex on line " << lineNum << endl;
    }
    int w;
    if ((line >> w)) {
        cout << "ERROR: w was specified on line " << lineNum << endl;
    }
    return v;
}

// TODO redo face system to not copy everything so much
// f vi/ti/ni .....
void ObjLoader::parseFace(istringstream &line, int lineNum) {
    vector<int> face;
    string chunk;	// One v/vn/vt component of the line
    while (line >> chunk) {
        //grab a chunk
        // try to split chunk into vt/vn
        size_t loc;
        size_t prev = 0;
        int components = 0;
        while ((loc = chunk.find_first_of('/', prev)) != std::string::npos) {
            if (loc > prev) {
                face.push_back(std::stoi(string(chunk, prev, loc)));
            }
            components++;
        }
        // case with only vertex index
        if (loc == prev == 0) {
            face.push_back(std::stoi(chunk));
            components++;
        }
        if (components == 0) {
            cout << "ERROR: missing face elements on face tag line " << lineNum << endl;
        }
        // fill out missing components with 0s
        for (; components < 3; components++) {
            face.push_back(0);
        }
    }

    // Check for duplicated indices.
    /*
    Index index;
    index.vertex = obj.vertices[face[0]];
    index.texCoord = obj.texCoords[face[1]];
    index.normal = obj.normals[face[2]];

    // if the deduped array is empty, just insert.  Otherwise if we get 0 for the index value, we would need to only check that value != known index 0.
    // or is find + insert faster?
    // compare to vertices[0] if not empty 
    // inserting into vertices means adding a new entry to deduplicated vertices.

    int& vIndex = obj.deduplicatedVertices[obj.vertices[face[0] - 1]];
    int& nIndex = obj.deduplicatedNormals[obj.normals[face[0] - 1]];
    int& tIndex = obj.deduplicatedTexCoords[obj.texCoords[face[0] - 1]];



    if (iter == obj.deduplicatedVertices.end()) {

    }

    iter = obj.deduplicatedTexCoords.find(obj.texCoords[face[1] - 1]);
    if (iter == obj.deduplicatedTexCoords.end()) {

    }
    iter = obj.deduplicatedNormals.find(obj.normals[face[1] - 1]);
    if (iter == obj.deduplicatedNormals.end()) {

    }


    if (obj.deduplicatedIndices.find(index) == obj.deduplicatedIndices.end()) {
        obj.vertices.push_back(index.vertex);
        obj.normals.push_back(index.normal);
        obj.texCoords.push_back(index.texCoord);
        obj.deduplicatedIndices[index] = obj.vertices.size() - 1;

    }
    indices.push_back(deduplicatedIndices[index]);

    */
    for (Group * group : curState.activeGroups) {
        group->faces.push_back(face);
    }
}

void ObjLoader::parseGroup(istringstream &line, int lineNum) {
    curState.activeGroups.clear();

    string name;
    while ((line >> name)) {
        if (groups.find(name) == groups.end()) {
            groups.insert({ name, Group(name) });
        }
        curState.activeGroups.push_back(&groups.find(name)->second);
    }
    // an empty group line means we want the default group
    if (curState.activeGroups.empty()) {
        curState.activeGroups.push_back(&groups["default"]);
    }
}

void ObjLoader::printStats() {
    cout << "Materials: " << materials.size() << endl;
    cout << "Groups: " << groups.size() << endl;
    int vertices = 0;
    int faces = 0;
    for (std::pair<string, Group> group : groups) {
        faces += (int)group.second.faces.size();
    }
    cout << "Vertices: " << obj.vertices.size() << endl;
    cout << "Faces: " << faces << endl;
}

/*
// Crappy method that deduplicates after load into unified groups.
void ObjLoader::deduplicateGroups(vector<glm::vec3> vertices, 
    vector<glm::vec3> &normals, 
    vector<glm::vec2> &texCoords, 
    vector<ObjMesh> &unifiedGroups) {

    for (unordered_map<string, Group>::const_iterator iter = groups.begin(); iter != groups.end(); ++iter) {
        const Group& group = iter->second;

        int curIndex = 0;
        vector<int> indices;
        unordered_map<Index, int> deduplicatedIndices;
        for (int faceI = 0; faceI < group.faces.size(); faceI++) {
            vector<int> face = group.faces[faceI];
            Index index;
            index.vertex = obj.vertices[face[0]];
            index.texCoord = obj.texCoords[face[1]];
            index.normal = obj.normals[face[2]];

            if (deduplicatedIndices.find(index) == deduplicatedIndices.end()) {
                deduplicatedIndices[index] = curIndex++;
                vertices.push_back(index.vertex);
                normals.push_back(index.normal);
                texCoords.push_back(index.texCoord);
            }
            indices.push_back(deduplicatedIndices[index]);
        }
        // place data into new grouping by material with unified index.
        ObjMesh unifiedGroup;
        unifiedGroup.indices = indices;
        unifiedGroup.groupName = group.groupName;
        unifiedGroup.material = group.
        // TODO material?

        unifiedGroups.push_back(unifiedGroup);

        // todo split materials into subgroups since groupings for the engine should be organized by material

    }
}
*/

// TODO list for this crap:
// fix material to not be ptr.  just copy the fucking thing
// where is the material in unified group?