#pragma once

#include "../Memory/Handle.h"

struct Model {
    Handle vertexBufferHandle;
    Handle normalHandle;
    Handle texCoordsHandle;
    Handle indicesHandle;
    // to allow contiguous storage of related meshes, knowing only the number of meshes is necessary. 
    // This way the model pool can allocate storage together for a given mesh
    // Model | Mesh0 | ... | Meshn
    int numMeshes; 


    // TODO - what can be used to referene the fact that the assets are loaded and ready to be processed?  Is that necessary?
};