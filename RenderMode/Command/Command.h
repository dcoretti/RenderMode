#pragma once

#include "../Dispatch/RenderApiDispatch.h"
//#include "Types\Handle.h"

/*
    General Linked set of commands, each with a set of 
*/
struct Command {
    typedef void(*DispatchCommand)(const void * data, const RenderApiDispatch & dispatch);

    // Dispatch function ptr that knows how to decode data and send it to the API level render api
    DispatchCommand dispatch{ nullptr };
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

void dispatchDrawVertexBuffer(const void * data, const RenderApiDispatch & dispatch);
struct DrawVertexBufferCommand : public CommandData < DrawVertexBufferCommand > {
    //unsigned int vaoId;
    int numVertices;
    int first;
    //VertexBufferHandle handle;
};
const Command::DispatchCommand CommandData<DrawVertexBufferCommand>::dispatchFn = &dispatchDrawVertexBuffer;


/*
* Use a shader program
*/
void dispatchSetShaders(const void * data, const RenderApiDispatch & dispatch);
struct SetShadersCommand : public CommandData < SetShadersCommand > {
    unsigned int programId;
};
const Command::DispatchCommand CommandData<SetShadersCommand>::dispatchFn = &dispatchSetShaders;

// TODO
//  set lighting params cmd, set uniforms, etc