#pragma once

#include "../Memory/Handle.h"

struct Model {
    Handle vertexBufferHandle;
    Handle normalHandle;
    Handle texCoordsHandle;
    Handle indicesHandle;
};