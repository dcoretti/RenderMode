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


/*
    General Linked set of commands, each with a set of 
*/
struct Command {
    typedef void(*DispatchCommand)(const void * data, RenderContext & context);

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
    int tmp;
};


/////////////////////////////////////
// commands

// Attached to 1 or more EnableVertexArrayCommands
void dispatchDrawVertexArray(const void * data, RenderContext &context);
struct DrawVertexArrayCommand : public CommandData <DrawVertexArrayCommand> {
    DrawVertexArrayCommand(GPU::VertexArrayObject vao, GPU::DrawContext drawContext) :
        vao(vao), drawContext(drawContext) {}
    DrawVertexArrayCommand() = default;

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
    SetShaderProgramCommand(Handle shaderProgramHandle) : shaderProgramHandle(shaderProgramHandle) {}
    SetShaderProgramCommand() = default;

    Handle shaderProgramHandle;

    // For shader loading:
    // TODO: how to map attribute layout to attrib array when drawing vertex,normal, etc.
    // For GL this is     glEnableVertexAttribArray(INDEX);
    // in the shader the layout specifies that via layout (location = 0) in vec4 position;
    // Maybe use glGetAttribLocation to find layout of named attributes expected by the engine to automatically bind those
    // and move the attribute to an enum on the draw command to specify what type of data is being pointed at (vertex, normal, etc)
    // and figure out the glEnableVertexAttribArray from there.

    // or on load of shader, specify locations via glBindAttribLocation via common naming convention in the glsl
    // see: http://stackoverflow.com/questions/4635913/explicit-vs-automatic-attribute-location-binding-for-opengl-shaders
};
const Command::DispatchCommand CommandData<SetShaderProgramCommand>::dispatchFn = &dispatchSetShaderProgram;


// Load constant array buffer into the GPU (such as creating a GL_ARRAY_BUFFER)
void dispatchLoadArrayBuffer(const void * data, RenderContext & context);
struct LoadArrayBufferCommand : public CommandData<LoadArrayBufferCommand> {
    LoadArrayBufferCommand(bool isIndexArray,
                           SystemBuffer systemBuffer,
                           Handle geometryBuffer,
                           GPU::ShaderAttributeBinding shaderBinding,
                           GPU::GeometryBufferLayout bufferLayout):
        isIndexArray(isIndexArray),
        systemBuffer(systemBuffer),
        geometryBuffer(geometryBuffer),
        shaderBinding(shaderBinding),
        bufferLayout(bufferLayout){}

    LoadArrayBufferCommand() = default;

    bool isIndexArray;  // TODO maybe move this to a type if there is more than GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER used
    SystemBuffer systemBuffer;  // Data stored in system memory to be loaded (move to handle once impl uses that in ModelManager)

    // resulting GPU id will be placed in this location by command executor. Command creator must preallocate this in a pool
    Handle geometryBuffer;  
    GPU::ShaderAttributeBinding shaderBinding;   // type of data mapped to shader input poiint
    GPU::GeometryBufferLayout bufferLayout;
};
const Command::DispatchCommand CommandData<LoadArrayBufferCommand>::dispatchFn = &dispatchLoadArrayBuffer;


void dispatchCreateShader(const void *data, RenderContext & context);
struct CreateShaderCommand : public CommandData<CreateShaderCommand> {
    CreateShaderCommand(Handle shaderProgram, const char * shaderSourceData, GPU::ShaderType shaderType) :
        shaderProgram(shaderProgram), shaderSourceData(shaderSourceData), shaderType(shaderType){
    }
    CreateShaderCommand() = default;

    Handle shaderProgram;   // pre-allocated location for shader id.  Will be allocated by renderer upon creation.
    const char * shaderSourceData;    // pooled shader source code str
    GPU::ShaderType shaderType;
};
const Command::DispatchCommand CommandData<CreateShaderCommand>::dispatchFn = &dispatchCreateShader;



void initializeAndSetVertexArray(const void *data, RenderContext &context);
struct InitializeAndSetVertexArrayCommand : public CommandData<InitializeAndSetVertexArrayCommand> {
    Handle vertexArrayObject;
};
const Command::DispatchCommand CommandData<InitializeAndSetVertexArrayCommand>::dispatchFn = &initializeAndSetVertexArray;