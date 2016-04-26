#pragma once
#include "Types\Application\Font\Font.h"

namespace FontTest {
    void testFont() {
        int texW = 256;
        int texH = 128;
        unsigned char * texture = new unsigned char[texW * texH]{ 0 };
        Text::Font f{};
        f.heightPixels = 32.0f;

        Text::loadFontToTexture(f, "C:/Windows/Fonts/Arial.ttf", texture, texW, texH);

        delete[] texture;
    }

}