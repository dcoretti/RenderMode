#pragma once

#include <glm/vec3.hpp>
#include "../Memory/Handle.h"

// Note: put material in the key so we don't have to have a setMaterial command.  instead sorting is used internally by 
// the rendering layer to determine which material is active using the key and a id->material map
struct Material {
    glm::vec3 ambient;   // ka
    glm::vec3 diffuse;   // kd
    glm::vec3 specular;  // ks

    float dissolve;
    int illum; // illumination model #

    Handle diffuseTexture;
    Handle ambientTexture;
    Handle specularTexture;
};


