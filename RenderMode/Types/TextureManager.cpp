#include "TextureManager.h"
#include "../Util/Common.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Types/Application/stb_image.h"


Handle TextureManager::loadTexture(const char * fname, CommandBuilder &cmdBuilder, RenderQueue &renderQueue) {
    GPU::TextureBufferLayout layout;
    SystemBuffer buffer;
    int components;
    int requestedFormat = 0;
    buffer.data = stbi_loadf(fname, &layout.width, &layout.height, &components, requestedFormat);
    DBG_ASSERT(buffer.data != nullptr, "Problem loading file %s", fname);

    buffer.sizeBytes = components * layout.width * layout.height;
    Handle h = submitTexture(buffer, layout,cmdBuilder, renderQueue);
    stbi_image_free(buffer.data);
    return h;
}

Handle TextureManager::submitTexture(SystemBuffer &buffer, GPU::TextureBufferLayout &bufferLayout, CommandBuilder &cmdBuilder, RenderQueue &renderQueue) {
    Handle bufferObject = bufferObjectPool->createObject();
    Handle loadCmd = cmdBuilder.buildLoadTextureCommand(buffer, bufferLayout, bufferObject);
    renderQueue.submit(loadCmd, CommandKey());
    renderQueue.execute();
    return bufferObject;
}

