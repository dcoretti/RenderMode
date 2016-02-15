#pragma once


/*
    Defines global binding points to shader.  Requires that all shaders use this global data layout for input
    arrays.  Used by load commands to bind buffers to layout points when defining a vertex array object
*/
namespace GPU {
    // or on load of shader, specify locations via glBindAttribLocation via common naming convention in the glsl
    // see: http://stackoverflow.com/questions/4635913/explicit-vs-automatic-attribute-location-binding-for-opengl-shaders
    enum ShaderAttributeBinding {
        VERTICES = 0,
        UV = 1,
        NORMALS = 2,
    };

    struct ShaderProgram {
        unsigned int shaderProgramId; // compiled program id for a shader program 

        // Shaders linked to shaderProgramId
        unsigned int vertexShader;
        unsigned int fragmentShader;
    };


    enum class ShaderType {
        Vertex,
        Fragment
    };



    struct ShaderData {
        char * source;
        // maybe add this back later?
        //ShaderType type; 
    };

    const unsigned int numShaderAttributeComponents = 3;
}
