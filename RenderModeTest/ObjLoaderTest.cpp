#include "gtest/gtest.h"

#include "Util/ObjLoader.h"

#include <sstream>
#include <string>

using std::string;
using std::istringstream;

Group& getGroup(ModelObject &obj, const std::string &name) {
    EXPECT_TRUE(obj.groups.find(name) != obj.groups.end());
    return obj.groups[name];
}

Group& getDefaultGroup(ModelObject& obj) {
    return getGroup(obj, ObjLoader::defaultGroupName);
}

TEST(ObjLoaderTest, emptyObj) {
    string input = "";
    istringstream is(input);

    ModelObject obj = ObjLoader::load(is);
    EXPECT_EQ(0, obj.normals.size());
    EXPECT_EQ(0, obj.vertices.size());
    EXPECT_EQ(0, obj.texCoords.size());
    EXPECT_EQ(1, obj.materials.size());
    
}

TEST(ObjLoaderText, junkLine) {
    string input = "asdf 1 2 3";
    istringstream is(input);

    ModelObject obj = ObjLoader::load(is);
    EXPECT_EQ(0, obj.normals.size());
    EXPECT_EQ(0, obj.vertices.size());
    EXPECT_EQ(0, obj.texCoords.size());
    EXPECT_EQ(1, obj.materials.size());
}

TEST(ObjLoaderTest, vertexParse) {
    string input = "v 1 2 3";
    istringstream is(input);

    ModelObject obj = ObjLoader::load(is);
    EXPECT_EQ(0, obj.normals.size());
    EXPECT_EQ(1, obj.vertices.size());
    EXPECT_EQ(0, obj.texCoords.size());
    EXPECT_EQ(1, obj.materials.size());

    EXPECT_EQ(glm::vec3(1, 2, 3), obj.vertices[0]);
}

TEST(ObjLoaderTest, normalParse) {
    string input = "vn 1 2 3";
    istringstream is(input);

    ModelObject obj = ObjLoader::load(is);
    EXPECT_EQ(1, obj.normals.size());
    EXPECT_EQ(0, obj.vertices.size());
    EXPECT_EQ(0, obj.texCoords.size());
    EXPECT_EQ(1, obj.materials.size());

    EXPECT_EQ(glm::vec3(1, 2, 3), obj.normals[0]);
}

TEST(ObjLoaderTest, texParse) {
    string input = "vt 1 2";
    istringstream is(input);

    ModelObject obj = ObjLoader::load(is);
    EXPECT_EQ(0, obj.normals.size());
    EXPECT_EQ(0, obj.vertices.size());
    EXPECT_EQ(1, obj.texCoords.size());
    EXPECT_EQ(1, obj.materials.size());

    EXPECT_EQ(glm::vec2(1, 2), obj.texCoords[0]);
}

TEST(ObjLoaderTest, faceIndexZeroFill) {
    string input =
        "v 1 2 3\n"
        "vn 1 1 1\n"
        "vt 0.1 0.1 0.2\n"
        "f 1";
    ModelObject obj = ObjLoader::load(istringstream(input));
    Group g = getDefaultGroup(obj);
    EXPECT_EQ(1, g.faces.size());
    vector<int> expected{ 1, 0, 0 };    // zeroes filled in
    EXPECT_EQ(expected, g.faces[0]);
}

TEST(ObjLoaderTest, faceParse) {
    string input = "v 1 2 3\nf 1";
    istringstream is(input);

    ModelObject obj = ObjLoader::load(is);
    EXPECT_EQ(0, obj.normals.size());
    EXPECT_EQ(1, obj.vertices.size());
    EXPECT_EQ(0, obj.texCoords.size());
    EXPECT_EQ(1, obj.materials.size());

    EXPECT_EQ(glm::vec3(1, 2, 3), obj.vertices[0]);
    EXPECT_EQ(1, obj.groups.size());

    Group g = obj.groups[ObjLoader::defaultGroupName];
    EXPECT_EQ(1, g.faces.size());
}

TEST(ObjLoaderTest, faceMultiIndex) {
    string input =
        "v 1 2 3\n"
        "vn 1 1 1\n"
        "vt 0.1 0.1 0.2\n"
        "f 1/1/1";
    ModelObject obj = ObjLoader::load(istringstream(input));

    EXPECT_EQ(1, obj.normals.size());
    EXPECT_EQ(1, obj.vertices.size());
    EXPECT_EQ(1, obj.texCoords.size());

    Group g = obj.groups[ObjLoader::defaultGroupName];
    vector<int> expected{ 1, 1, 1 };
    EXPECT_EQ(expected, g.faces[0]);
}

TEST(ObjLoaderTest, groupParse) {
    string groupName = "mygroup";
    string input = "g " + groupName;
    ModelObject obj = ObjLoader::load(istringstream(input));

    EXPECT_EQ(2, obj.groups.size());
    EXPECT_TRUE(obj.groups.find(groupName) != obj.groups.end());
    EXPECT_TRUE(obj.groups.find(ObjLoader::defaultGroupName) != obj.groups.end());
    EXPECT_EQ(0, obj.groups[groupName].faces.size());
    EXPECT_EQ(groupName, obj.groups[groupName].groupName);
}

TEST(ObjLoaderTest, usemtlForNotYetLoadedMtl) {
    string mtlName = "myMtl";
    string input = "usemtl " + mtlName;

    ModelObject obj = ObjLoader::load(istringstream(input));
    EXPECT_EQ(2, obj.materials.size());
    EXPECT_TRUE(obj.materials.find(mtlName) != obj.materials.end());
    EXPECT_EQ(mtlName, obj.materials[mtlName].name);
}

TEST(ObjLoaderTest, multiFaceContext) {
    string mtlName = "mtl1";
    string mtlName2 = "mtl2";

    string input =
        "v 1 2 3\n"
        "v 4 5 6\n"
        "v 7 8 9\n"
        "usemtl " + mtlName + "\n"
        "f 1\n"
        "f 3\n"
        "usemtl " + mtlName2 + "\n"
        "f 2\n";
    ModelObject obj = ObjLoader::load(istringstream(input));
    EXPECT_EQ(1, obj.groups.size());
    EXPECT_EQ(3, obj.materials.size());
    EXPECT_EQ(3, getDefaultGroup(obj).faces.size());
    EXPECT_EQ(2, getDefaultGroup(obj).materialStates.size());

    // each material has the correct starting face index.
    EXPECT_EQ(0, getDefaultGroup(obj).materialStates[0].faceIndex);
    EXPECT_EQ(2, getDefaultGroup(obj).materialStates[1].faceIndex);
}
