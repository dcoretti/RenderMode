#pragma once

#include <glm/vec3.hpp>

/*
    Defines global binding points to shader.  Requires that all shaders use this global data layout for input
    arrays.  Used by load commands to bind buffers to layout points when defining a vertex array object
*/
namespace GPU {
    // Todo move to more dynamic struct handling system to support arrays of structs
    // as uniforms (Light[i].color etc.)
    namespace Uniform {
        /* Uniform names in glsl source */
        static const char * mvp = "mvp";
        static const char * mv = "mv";
        static const char * v = "v";
        static const char * m = "m";

        // Material components
        static const char * materialBlockIndex = "Material";
        static const char * lightSourceBlockIndex = "LightSource";

        // Default bindings between shader programs for uniform blocks.  see populateShaderUniformLocations
        const int defaultMaterialUniformBlockBinding= 0;
        const int defaultLightSourceUniformBlockBinding = 1;

        // Texture uniforms
        static const char * diffuseTexture = "diffuseTexture";
        static const char * normalMapTexture = "normalMap";

        /*
        1:1 correspondence to constants below.
        Add new uniforms to the RenderApiDispatch populateShaderUniformLocations method
        */
        struct ShaderUniformLocations {
            int mvp{ -1 };
            int mv{ -1 };
            int v{ -1 };
            int m{ -1 };
            int diffuseTexture{ -1 };
            int normalMapTexture{ -1 };

            int lightSourceUniformBlockIndex{ -1 };
            int materialUniformBlockIndex{ -1 };
        };

        struct LightSource {
            glm::vec3 diffuse;
            glm::vec3 specular;
            glm::vec3 location;

            // TODO directional lighting?
        };

        struct Material {
            glm::vec3 ambient;   // ka
            glm::vec3 diffuse;   // kd
            glm::vec3 specular;  // ks
        };

    }


    // or on load of shader, specify locations via glBindAttribLocation via common naming convention in the glsl
    // see: http://stackoverflow.com/questions/4635913/explicit-vs-automatic-attribute-location-binding-for-opengl-shaders
    enum ShaderAttributeBinding {
        VERTICES = 0,
        UV = 1,
        NORMALS = 2,
    };

    enum class ShaderType {
        Vertex,
        Fragment
    };

    struct ShaderData {
        const char * source;
        // maybe add this back later?
        //ShaderType type; 
    };

    const unsigned int numShaderAttributeComponents = 3;
    struct ShaderProgram {
        unsigned int shaderProgramId; // compiled program id for a shader program 

                                      // Shaders linked to shaderProgramId
        unsigned int vertexShader;
        unsigned int fragmentShader;
        Uniform::ShaderUniformLocations uniformLocations;
    };
}
