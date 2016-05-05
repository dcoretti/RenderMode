#pragma once

#include "../Types/CommonTypes.h"
#include "../Memory/Handle.h"
#include "../Types/GPU//GeometryTypes.h"
#include "../Command/CommandBuilder.h"
#include "../Render/RenderQueue.h"
class TextureManager {
    Handle loadTexture(const char * fname, CommandBuilder &cmdBuilder, RenderQueue &renderQueue);
    Handle submitTexture(SystemBuffer &buffer, GPU::TextureBufferLayout &bufferLayout, CommandBuilder &cmdBuilder, RenderQueue &renderQueue);

    IndexedPool<GPU::BufferObject> *bufferObjectPool;
};