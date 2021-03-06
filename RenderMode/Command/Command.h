#pragma once

#include "../Memory/Handle.h"
#include "../Types/GPU/GeometryTypes.h"
#include "../Types/GPU/ShaderTypes.h"
#include "../Render/RenderContext.h"
#include "../Types/CommonTypes.h"
/*
    Requirements for commands:
    -Commands should be plain old data.  No manipulation of the data should be done at this point.  The data
        is just arguments to some graphics API fuction.
    -Avoid multiple definitions of data.  Favor using one structure containing the data rather than copying the 
        struct over to an unpacked version in the command data.  This will make updating the data quicker since there
        is only one place to update it.
    -Handles are used when data is updated (generating a bufferid).  Use the object behind it when a const is expected.
*/

//TODO probably a good idea to ditch the constructors to keep these PODS

/*
    General Linked set of commands, each with a set of 
*/
struct Command {
    typedef void(*DispatchCommand)(const void *data, RenderContext &context);

    // Dispatch function ptr that knows how to decode data and send it to the API level render api
    DispatchCommand dispatch{ nullptr };

    Command * next{ nullptr }; // next command in a group
    char * extraCmdData;    // Extra data such as shader uniforms, etc.  
};

/*
 Each cmd data derives from this and specifies the loation of the dispatch fn.
 Curiously recurring template pattern - each derived class has its own static dispatch Fn!!
 declared as class Derived: public CommandData<Derived>
 Each command has a dispatch Fn and arbitrary data.
*/
template <class Derived>
struct CommandData {
    static const Command::DispatchCommand dispatchFn;
    // TODO how to assert per subtype that dispatchFn != nullptr?
};

struct CommandKey {
    CommandKey() = default; // TODO remove this when command keys implemented
    uint16_t materialId;
};


/////////////////////////////////////////////////////////////////////////////////////////////
// Commands
/////////////////////////////////////////////////////////////////////////////////////////////

// Attached to 1 or more EnableVertexArrayCommands
void dispatchDrawVertexArray(const void * data, RenderContext &context);
struct DrawVertexArrayCommand : public CommandData <DrawVertexArrayCommand> {
    DrawVertexArrayCommand(bool indexed, GPU::VertexArrayObject vao, GPU::DrawContext drawContext) :
        indexed(indexed), vao(vao), drawContext(drawContext) {}
    DrawVertexArrayCommand() = default;
    bool indexed;   // if indexed, drawContext refers to the dimensions of the index array.
    GPU::DrawContext drawContext;
    GPU::VertexArrayObject vao;
};

const Command::DispatchCommand CommandData<DrawVertexArrayCommand>::dispatchFn = &dispatchDrawVertexArray;


// no longer needed???
//void dispatchEnableVertexArray(const void * data, RenderContext & context);
//struct EnableVertexArrayCommand : public CommandData <EnableVertexArrayCommand> {
//    //ShaderAttributeLayout attributeType;
//    unsigned int size;
//    int stride{ 0 };
//};
//const Command::DispatchCommand CommandData<EnableVertexArrayCommand>::dispatchFn = &dispatchEnableVertexArray;


/*
* Use a shader program.  Auxilary memory should hold uniform key/values for submission.
*/
void dispatchSetShaderProgram(const void * data, RenderContext & context);
struct SetShaderProgramCommand : public CommandData <SetShaderProgramCommand> {
    // Maybe use handle here otherwise load has to be executed first so that this isn't a copy of
    // the pre-initialized value
    GPU::ShaderProgram shaderProgram;  
 };
const Command::DispatchCommand CommandData<SetShaderProgramCommand>::dispatchFn = &dispatchSetShaderProgram;


void dispatchLoadTextureBuffer(const void *data, RenderContext &context);
struct LoadTextureBufferCommand : public CommandData<LoadTextureBufferCommand> {
    Handle textureBuffer;   // to texture buffer id   
    SystemBuffer systemBuffer;
    GPU::TextureBufferLayout textureBufferLayout;
};
const Command::DispatchCommand CommandData<LoadTextureBufferCommand>::dispatchFn = &dispatchLoadTextureBuffer;


// Load constant array buffer into the GPU (such as creating a GL_ARRAY_BUFFER)
void dispatchLoadArrayBuffer(const void * data, RenderContext & context);
struct LoadArrayBufferCommand : public CommandData<LoadArrayBufferCommand> {
    LoadArrayBufferCommand(SystemBuffer systemBuffer,
                           Handle geometryBuffer,
                           GPU::ShaderAttributeBinding shaderBinding,
                           GPU::GeometryBufferLayout bufferLayout):
        systemBuffer(systemBuffer),
        geometryBuffer(geometryBuffer),
        shaderBinding(shaderBinding),
        bufferLayout(bufferLayout){}

    LoadArrayBufferCommand() = default;

    SystemBuffer systemBuffer;  // Data stored in system memory to be loaded (move to handle once impl uses that in ModelManager)

    // resulting GPU id will be placed in this location by command executor. Command creator must preallocate this in a pool
    Handle geometryBuffer;  
    GPU::ShaderAttributeBinding shaderBinding;   // type of data mapped to shader input poiint
    GPU::GeometryBufferLayout bufferLayout;
};
const Command::DispatchCommand CommandData<LoadArrayBufferCommand>::dispatchFn = &dispatchLoadArrayBuffer;

void dispatchLoadIndexArrayBuffer(const void * data, RenderContext & context);
struct LoadIndexArrayBufferCommand : public CommandData<LoadIndexArrayBufferCommand> {
    Handle indexGeometryBuffer; // handle to index buffer in geometry pool
    SystemBuffer systemBuffer;
};
const Command::DispatchCommand CommandData<LoadIndexArrayBufferCommand>::dispatchFn = &dispatchLoadIndexArrayBuffer;

void dispatchCreateShader(const void *data, RenderContext & context);
struct CreateShaderCommand : public CommandData<CreateShaderCommand> {
    CreateShaderCommand(Handle shaderProgram, GPU::ShaderData vertexShaderData, GPU::ShaderData fragmentShaderData)
        :shaderProgram(shaderProgram), vertexShaderData(vertexShaderData), fragmentShaderData(fragmentShaderData) {}
    CreateShaderCommand() = default;

    Handle shaderProgram;   // pre-allocated location for shader id.  Will be allocated by renderer upon creation.
    GPU::ShaderData vertexShaderData;
    GPU::ShaderData fragmentShaderData;
};
const Command::DispatchCommand CommandData<CreateShaderCommand>::dispatchFn = &dispatchCreateShader;

void dispatchInitializeAndSetVertexArray(const void *data, RenderContext &context);
struct InitializeAndSetVertexArrayCommand : public CommandData<InitializeAndSetVertexArrayCommand> {
    Handle vertexArrayObject;
};
const Command::DispatchCommand CommandData<InitializeAndSetVertexArrayCommand>::dispatchFn = &dispatchInitializeAndSetVertexArray;


/* Shader Uniform Commands */
void dispatchSetMatrixUniform(const void * data, RenderContext &context);
struct SetMatrixUniformCommand : public CommandData<SetMatrixUniformCommand> {
    float *matrix;
    bool transpose;
    int numMatrices;
    int uniformLocation;
};
const Command::DispatchCommand CommandData<SetMatrixUniformCommand>::dispatchFn = &dispatchSetMatrixUniform;

void dispatchSetVec3UniformCommand(const void * data, RenderContext &context);
struct SetVec3UniformCommand : public CommandData<SetVec3UniformCommand> {
    float x, y, z;
    int uniformLocation;
};
const Command::DispatchCommand CommandData<SetVec3UniformCommand>::dispatchFn = &dispatchSetVec3UniformCommand;

void dispatchSetFloatUniformCommand(const void * data, RenderContext &context);
struct SetFloatUniformCommand : public CommandData<SetFloatUniformCommand> {
    float * vals;
    int count;
    int uniformLocation;
};
const Command::DispatchCommand CommandData<SetFloatUniformCommand>::dispatchFn = &dispatchSetFloatUniformCommand;


void dispatchInitializeUniformBuffer(const void * data, RenderContext &context);
struct InitializeUniformBufferCommand : public CommandData<InitializeUniformBufferCommand> {
    Handle uboHandle;
    size_t bufferSize;
    void * data;    // optional.  If set later during a mapBuffer command can be null.
    int bufferBlockBinding;
};
const Command::DispatchCommand CommandData<InitializeUniformBufferCommand>::dispatchFn = &dispatchInitializeUniformBuffer;

void dispatchUpdateUniformBuffer(const void *data, RenderContext &context);
struct UpdateUniformBufferCommand : public CommandData<UpdateUniformBufferCommand> {
    GPU::UniformBufferObject ubo;
    void * data;
    size_t bufferSize;
    size_t offset;
};
const Command::DispatchCommand CommandData<UpdateUniformBufferCommand>::dispatchFn = &dispatchUpdateUniformBuffer;


void dispatchSetTexture(const void * data, RenderContext &context);
struct SetTextureCommand : public CommandData<SetTextureCommand> {
    GPU::TextureBufferObject textureBufferObj;
    unsigned int textureBufferSlot;  // max is GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
    int uniformLocation;
};
const Command::DispatchCommand CommandData<SetTextureCommand>::dispatchFn = &dispatchSetTexture;