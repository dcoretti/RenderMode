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


    enum class ShaderType {
        Vertex,
        Fragment
    };

    struct ShaderData {
        char * source;
        // maybe add this back later?
        //ShaderType type; 
    };

    // Todo move to more dynamic struct handling system to support arrays of structs
    // as uniforms (Light[i].color etc.)
    namespace Uniform {
        /*  
            1:1 correspondence to constants below.  
            Add new uniforms to the RenderApiDispatch populateShaderUniformLocations method 
        */
        struct ShaderUniformLocations {
            int mvp{ -1 };
            int lightcolor{ -1 };
            int matAmbient{ -1 };
            int matDiffuse{ -1 };
            int matSpecular{ -1 };
            int diffuseTexture{ -1 };
            int normalMapTexture{ -1 };
        };
        static const char * mvp = "mvp";

        // Material components
        static const char * lightColor = "lightColor";
        
        static const char * matAmbient = "matAmbient";
        static const char * matDiffuse = "matDiffuse";
        static const char * matSpecular = "matSpecular";

        // Texture uniforms
        static const char * diffuseTexture = "diffuseTexture";
        static const char * normalMapTexture = "normalMap";
        
    }
    const unsigned int numShaderAttributeComponents = 3;


    struct ShaderProgram {
        unsigned int shaderProgramId; // compiled program id for a shader program 

                                      // Shaders linked to shaderProgramId
        unsigned int vertexShader;
        unsigned int fragmentShader;
        Uniform::ShaderUniformLocations uniformLocations;
    };

}
