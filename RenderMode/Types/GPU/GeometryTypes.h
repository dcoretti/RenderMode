#pragma once
/*
    General geometric data types used together to construct render engine commands for GPU loaded data.
*/
namespace GPU {
    /*
    container for buffer id to identify it on gpu for vertex, texture, normal, index buffers
    used in draw commands for static buffered objects on the gpu
    */
    struct GeometryBuffer {
        unsigned int bufferId;  // GPU id such as one generated by glGenBuffers
    };


    /*
        Layout of memory in a given buffer used for vertices, normals, etc.
        general components used in glAttributPointer
    */
    struct GeometryBufferLayout {
        size_t componentSize;   // vec3, etc.
        unsigned int numComponents; // x,y,z = 3.  size input to glVertexAttribPointer
        unsigned int stride;
        // type?
    };

    struct Texture {
        unsigned int textureId; // GPU id generated by api such as glGenTextures
    };

    struct ShaderProgram {
        unsigned int shaderProgramId; // compiled program id for a shader program 
    };
}
