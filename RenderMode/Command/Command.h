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

void dispatchDrawIndexedVertexBuffer(const void * data, const RenderApiDispatch & dispatch);
struct DrawIndexedVertexBufferCommand : public CommandData <DrawIndexedVertexBufferCommand> {
    //unsigned int vaoId;
    int numVertices;
    int first;
    //VertexBufferHandle handle;
};
const Command::DispatchCommand CommandData<DrawIndexedVertexBufferCommand>::dispatchFn = &dispatchDrawIndexedVertexBuffer;


/*
* Use a shader program.  Auxilary memory should hold uniform key/values for submission.
*/
void dispatchSetShaderProgram(const void * data, const RenderApiDispatch & dispatch);
struct SetShaderProgramCommand : public CommandData <SetShaderProgramCommand> {
    unsigned int programId;
};
const Command::DispatchCommand CommandData<SetShaderProgramCommand>::dispatchFn = &dispatchSetShaderProgram;


// Load constant array buffer into the GPU (such as creating a GL_ARRAY_BUFFER)
void dispatchLoadArrayBuffer(const void * data, const RenderApiDispatch & dispatch);
struct LoadArrayBufferCommand : public CommandData<LoadArrayBufferCommand> {
    void * systemBuffer;  // Data stored in system memory to be loaded (move to handle once impl uses that in ModelManager)

    Handle loadedBufferHandle;  // resulting data will be placed in this location
    unsigned int vertexCount;
    unsigned int vertexComponents;
    unsigned int stride;

};
const Command::DispatchCommand CommandData<LoadArrayBufferCommand>::dispatchFn = &dispatchLoadArrayBuffer;


// Load constant index buffer into the gpu (GL_ELEMENT_ARRAY_BUFFER for GL)
void dispatchLoadIndexArrayBuffer(const void * data, const RenderApiDispatch & dispatch);
struct LoadIndexArrayBufferCommand : public CommandData<LoadIndexArrayBufferCommand> {
    Handle bufferHandle; // handle to data in local memory pool
    Handle loadedBufferHandle;  // resulting data will be placed in this location including the id

    unsigned int indexCount;
    unsigned int offset;
};
const Command::DispatchCommand CommandData<LoadIndexArrayBufferCommand>::dispatchFn = &dispatchLoadIndexArrayBuffer;


