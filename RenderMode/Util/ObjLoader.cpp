#include "ObjLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iterator>

using std::stringstream;
using std::ifstream;
using std::getline;
using std::cout;
using std::endl;
using std::iterator;

const string ObjLoader::defaultGroupName = "default";
const string ObjLoader::defaultMaterialName = "Default";

void initState(State &state, ModelObject &obj) {
    glm::vec3 w;
    w.x = 1.0f;
    w.y = 1.0f;
    w.z = 1.0f;
    ObjMaterial white = obj.materials[ObjLoader::defaultMaterialName];
    white.ka = w;
    white.kd = w;
    white.ks = w;
    white.dissolve = 1.0f;
    state.curMaterial = &white;

    Group &defaultGroup = obj.groups[ObjLoader::defaultGroupName];	// spec default group name
    defaultGroup.groupName = ObjLoader::defaultGroupName;

    state.activeGroups.push_back(&defaultGroup);
}

ModelObject ObjLoader::load(std::string fname) {
    cout << "loading " << fname << endl;
    ifstream in(fname);
    if (!in.is_open()) {
        cout << "unable to load file " << fname << endl;
        return ModelObject();
    }

    ModelObject obj = load(in);
    in.close();
    return obj;
}

ModelObject ObjLoader::load(istream &in) {
    ModelObject obj;
    State curState;
    initState(curState, obj);


    string line;
    int lineNum = 0;
    while (getline(in, line)) {
        cout << "line: " << line << endl;
        if (line.length() == 0) {
            continue;
        }
        istringstream stream(line);
        string token;
        if (!(stream >> token)) {
            cout << "missing string token at beginning of line" << endl;
            return obj;
        }

        // Check top level organizational tags for major state changes
        if (token == "#") {
            // comment. skip
        }
        /* Grouping */
        else if (token == "o") {
            stream >> obj.name;
        } else if (token == "g") {
            parseGroup(obj, curState, stream, lineNum);
        }
        /* Render Attributes */
        else if (token == "mtllib") {
            loadMaterial(obj, stream, lineNum);
        } else if (token == "usemtl") {
            string materialName;
            stream >> materialName;
            applyMtl(obj, curState, materialName);
        } else if (!parseElement(curState, token, stream, lineNum) &&
            !parseVertexData(obj, token, stream, lineNum)) {
            cout << "ERROR PARSING LINE " << lineNum << ": " << line << endl;
        }

        lineNum++;
    }
    printStats(obj);
    return obj;
}

void ObjLoader::applyMtl(ModelObject &obj, State &curState, const string &materialName) {
    if (obj.materials.find(materialName) == obj.materials.end()) {
        cout << "material with name '" << materialName << "' doesn't exist.. initializing" << endl;
        obj.materials[materialName].name = materialName;
    } else {
        cout << "material " << materialName << " found. " << endl;
    }
    curState.curMaterial = &obj.materials[materialName];
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

bool ObjLoader::parseElement(State &curState, const string &token, istringstream &line, int lineNum) {
    /* Elements */
    // TODO points, lines
    if (token == "f") {
        parseFace(curState, line, lineNum);
        return true;
    }

    return false;
}

bool ObjLoader::parseVertexData(ModelObject &obj, const string &token, istringstream &line, int lineNum) {
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
void ObjLoader::loadMaterial(ModelObject &obj, istringstream &stream, int lineNum) {
    string matFile;
    if (!(stream >> matFile)) {
        cout << "missing material file name." << endl;
        return;
    }
    parseMtl(obj, matFile);
}

// load/parse a MTL file
void ObjLoader::parseMtl(ModelObject &obj, const string &fname) {
    cout << "Loading material file at '" << fname << "'" << endl;
    ifstream in(fname);
    if (!in.is_open()) {
        cout << "unable to load file " << fname << endl;
        return;
    }

    string line;
    int lineNum = 0;
    ObjMaterial *curMaterial;
    bool activeMtl = false;
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
            string name;
            if (!(stream >> name)) {
                cout << "missing expected material name on line " << lineNum << endl;
                return;
            }

            if (obj.materials.find(name) != obj.materials.end()) {
                cout << "material with name '" << name << "' already exists.. replacing" << endl;
            }

            curMaterial = &obj.materials[name];
            curMaterial->name = name;
            activeMtl = true;
        } 
        if (activeMtl) {
            if (token == "Ka") {
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
        } else {
            if (!activeMtl) {
                cout << "unexpected token.  Expected newmtl as first command at or before line " << line << endl;
            } else {
                cout << "ERROR Invalid token for line " << lineNum << ": " << line << endl;
            }
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
void ObjLoader::parseFace(State &curState, istringstream &line, int lineNum) {
    vector<int> face;
    string chunk;	// One v/vn/vt component of the line
    while (line >> chunk) {
        //grab a chunk and try to split chunk into v/vn/vt
        cout << "chunk " << chunk << endl;
        int components = 0;

        size_t delim1 = chunk.find_first_of('/');
        size_t delim2 = chunk.find_last_of('/');
        if (delim1 == string::npos) {
            if (chunk.size() > 0) {
                face.push_back(std::stoi(chunk));
                face.push_back(0);
                face.push_back(0);
            } else {
                cout << lineNum << " ERROR: missing face elements on face tag for chunk '" << chunk << endl;
            }
        } else if (delim1 == delim2) {
            // f #/#
            face.push_back(std::stoi(chunk.substr(0, delim1)));
            face.push_back(std::stoi(chunk.substr(delim1 + 1, chunk.size() - delim1)));
            face.push_back(0);
        } else {
            // f #/#/#
            face.push_back(std::stoi(chunk.substr(0, delim1)));
            face.push_back(std::stoi(chunk.substr(delim1 + 1, delim2 - delim1)));
            face.push_back(std::stoi(chunk.substr(delim2 + 1, chunk.size() - delim1)));
        }
    }
    for (Group * group : curState.activeGroups) {
        group->faces.push_back(face);
    }
}

void ObjLoader::parseGroup(ModelObject &obj, State &curState, istringstream &line, int lineNum) {
    curState.activeGroups.clear();

    string name;
    while ((line >> name)) {
        if (obj.groups.find(name) == obj.groups.end()) {
            obj.groups.insert({ name, Group(name) });
        }
        curState.activeGroups.push_back(&obj.groups.find(name)->second);
    }
    // an empty group line means we want the default group
    if (curState.activeGroups.empty()) {
        curState.activeGroups.push_back(&obj.groups["default"]);
    }
}

void ObjLoader::printStats(ModelObject &obj) {
    cout << "Materials: " << obj.materials.size() << endl;
    cout << "Groups: " << obj.groups.size() << endl;
    int vertices = 0;
    int faces = 0;
    for (auto group : obj.groups) {
        faces += (int)group.second.faces.size();
    }
    cout << "Vertices: " << obj.vertices.size() << endl;
    cout << "Faces: " << faces << endl;
}