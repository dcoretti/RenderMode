#pragma once

#include "../../Core/HashTable.h"
#include <cstdint>

namespace Text {

    struct Glyph {
        int codePoint;
        int advanceWidth; // not scaled by xScale
        int leftSideBearing;
        int width, height;

        // Location in the bitmap
        int bitmapX;
        int bitmapY;
    };

    class Font {
    public:
        Font() : kernTable(512), 
            baseline(0),
            scaleX(1.0f),
            scaleY(1.0f),
            heightPixels(0.0f),
            ascent(0),
            descent(0),
            lineGap(0),
            glyphs(nullptr) {
        }


        int baseline;
        float scaleX;
        float scaleY;

        // defined prior to rasterizing
        float heightPixels; // font height in pixels
        int ascent;
        int descent;
        int lineGap;

        Glyph *glyphs{ nullptr };
        HashTablei kernTable;
    };


    int getCodePointPairKey(int a, int b);

    void loadFontToTexture(Font &font, char * fname, unsigned char * texture, int texW, int texH);

    struct GlyphNode {
        GlyphNode() = default;
        ~GlyphNode() {
            if (child[0] != nullptr) {
                delete child[0];
            }
            if (child[1] != nullptr) {
                delete child[1];
            }
        }
        void initChildren(int firstHalfW, int firstHalfH, bool isChildTopBottom);

        // x,y relative to the whole grid, not just the parent
        // 0,0 is at the bottom left
        int x{ 0 };
        int y{ 0 };
        int width{ 0 };
        int height{ 0 };

        // child represents alternating upper/lower half and left/right half of area defined by the parent
        GlyphNode *child[2]{ nullptr, nullptr };
        Glyph *glyph{ nullptr };
    };

}