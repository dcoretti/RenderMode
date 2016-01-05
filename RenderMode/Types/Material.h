#pragma once

#include <glm/vec3.hpp>
#include "../Memory/Handle.h"

struct Material {
    glm::vec3 ambient;   // ka
    glm::vec3 diffuse;   // kd
    glm::vec3 specular;  // ks

    float dissolve;
    int illum; // illumination model #
    float Ns; // specular exponent

    Handle shaderProgramHandle;
};