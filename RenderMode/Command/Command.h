#pragma once

#include "../Memory/Handle.h"
#include "../Dispatch/RenderApiDispatch.h"

/*
    General Linked set of commands, each with a set of 
*/
struct Command {
    typedef void(*DispatchCommand)(const void * data, const RenderApiDispatch & dispatch);

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
};
// TODO how to assert per subtype that dispatchFn != nullptr?




/////////////////////////////////////
// commands

void dispatchDrawVertexBuffer(const void * data, const RenderApiDispatch & dispatch);
struct DrawIndexedBufferCommand : public CommandData <DrawIndexedBufferCommand> {

};
const Command::DispatchCommand CommandData<DrawIndexedBufferCommand>::dispatchFn = &dispatchDrawVertexBuffer;


/*
* Use a shader program.  Auxilary memory should hold uniform key/values for submission.
*/
void dispatchSetShaderProgram(const void * data, const RenderApiDispatch & dispatch);
struct SetShaderProgramCommand : public CommandData <SetShaderProgramCommand> {
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
void dispatchLoadArrayBuffer(const void * data, const RenderApiDispatch & dispatch);
struct LoadArrayBufferCommand : public CommandData<LoadArrayBufferCommand> {
    void * systemBuffer;  // Data stored in system memory to be loaded (move to handle once impl uses that in ModelManager)
    unsigned int systemBufferSize;
    unsigned int elementSize;

    bool isIndexArray;  // TODO maybe move this to a type if there is more than GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER used
    // resulting GPU id will be placed in this location by command executor. Command creator must preallocate this in a pool
    Handle geometryBuffer;  
};
const Command::DispatchCommand CommandData<LoadArrayBufferCommand>::dispatchFn = &dispatchLoadArrayBuffer;