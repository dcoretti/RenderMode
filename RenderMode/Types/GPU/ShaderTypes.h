#pragma once


/*
    Defines global binding points to shader.  Requires that all shaders use this global data layout for input
    arrays.  Used by load commands to bind buffers to layout points when defining a vertex array object
*/
namespace GPU {
    enum ShaderAttributeBinding {
        VERTICES = 0,
        UV = 1,
        NORMALS = 2,
    };

    struct ShaderProgram {
        unsigned int shaderProgramId; // compiled program id for a shader program 
    };


    enum class ShaderType {
        Vertex,
        Fragment
    };

}
const unsigned int numShaderAttributeComponents = 3;