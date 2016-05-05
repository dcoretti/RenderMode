#include "Font.h"
#include <stdio.h>
#include "../../../Util/Common.h"

#include <iostream>
#include <algorithm>
using std::endl;
using std::cout;

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"



namespace Text {

    // init children such that is child is a top/bottom split, the top half height is equal to firstHalfH.
    // If left/right split (isChildTopBottom == false) the left half width is equal to firstHalfW
    void GlyphNode::initChildren(int firstHalfW, int firstHalfH, bool isChildTopBottom) {
        child[0] = new GlyphNode();
        child[1] = new GlyphNode();

        if (isChildTopBottom) {
            child[0]->x = x;
            child[0]->y = y + height - firstHalfH;

            child[1]->x = x;
            child[1]->y = y;

            child[0]->width = width;
            child[1]->width = width;
            child[0]->height = firstHalfH;
            child[1]->height = height - firstHalfH;
        } else {
            child[0]->x = x;
            child[0]->y = y;

            child[1]->x = x + firstHalfW;
            child[1]->y = y;

            child[0]->width = firstHalfW;
            child[1]->width = width - firstHalfW;
            child[0]->height = height;
            child[1]->height = height;
        }
    }

    GlyphNode * insert(GlyphNode * tree, Glyph *glyph, bool topBottom) {
        // leaf
        if (tree->child[0] == nullptr && tree->child[1] == nullptr) {
            if (tree->width < glyph->width || tree->height < glyph->height) {
                return nullptr;
            }
            if (tree->glyph != nullptr) {
                return nullptr;
            }

            // either fits perfectly or needs to be split
            if (glyph->height == tree->height && glyph->width == tree->width) {
                // insert here!
                tree->glyph = glyph;
                return tree;
            } else {
                // split node into perfect size and remainder
                tree->initChildren(glyph->width, glyph->height, !topBottom);
                return insert(tree->child[0], glyph, !topBottom); // glyph will fit into left half
            }

        } else {
            // non-leaf
            GlyphNode *node = insert(tree->child[0], glyph, !topBottom);
            if (node != nullptr) {
                return node;
            } else {
                return insert(tree->child[1], glyph, !topBottom);
            }
        }
    }

    GlyphNode * insert(GlyphNode * tree, Glyph * glyph) {
        // All trees starts with top half bottom half level
        return insert(tree, glyph, false);
    }

    void packGlyphs(unsigned char *texture, int texW, int texH, Glyph* glyphs, int numGlyphs, stbtt_fontinfo *font, float scaleX, float scaleY) {
        GlyphNode *tree = new GlyphNode();
        tree->x = 0;
        tree->y = 0;
        tree->width = texW;
        tree->height = texH;

        // insert the first node
        DBG_ASSERT(texH >= glyphs[0].height, "texture not tall enough for largest character");
        DBG_ASSERT(texW >= glyphs[0].width, "texture not wide enough for largest character");

        for (int i = 0; i < numGlyphs; i++) {
            GlyphNode * node = insert(tree, &glyphs[i]);
            DBG_ASSERT(node != nullptr, "glyph could not fit!");
            if (node != nullptr) {
                node->glyph = &glyphs[i];
                glyphs[i].bitmapX = node->x;
                glyphs[i].bitmapY = node->y;

                // get the subset of the texture where the glyph is at the top left (bitmap 0,0 is top right)
                int offsetRow = texH - (node->y + glyphs[i].height);
                // subsection of the texture such that the current element is at the top left
                unsigned char * textureStart = texture + (offsetRow * texW + node->x);  


                stbtt_MakeCodepointBitmap(font, 
                    textureStart, 
                    glyphs[i].width, 
                    glyphs[i].height, 
                    texW, 
                    scaleX, 
                    scaleY, 
                    glyphs[i].codePoint);
                cout << "packed glyph " << (char)glyphs[i].codePoint << " at {" << node->x << ", " << node->y << "}" << endl;
            }
        }
        delete tree;
    }

    void loadFontToTexture(Font &font, char * fname, unsigned char * texture, int texW, int texH) {
        stbtt_fontinfo fontInfo;
        unsigned char *buffer  = new unsigned char[1024 * 1024 * 10];

        FILE * f = fopen(fname, "rb");
        DBG_ASSERT(f != NULL, "File unable to be opened");

        fread(buffer, 1, 1024*1024*10, f);
        fclose(f);

        stbtt_InitFont(&fontInfo, buffer, stbtt_GetFontOffsetForIndex(buffer, 0));
        font.scaleY = stbtt_ScaleForPixelHeight(&fontInfo, font.heightPixels);
        font.scaleX = font.scaleY;
        stbtt_GetFontVMetrics(&fontInfo, &font.ascent, &font.descent, &font.lineGap);

        font.baseline = font.ascent * (int)font.scaleY;

        char start = '!';
        char end = '~';
        int numGlyphs = end - start;
        Glyph *glyphs = new Glyph[end - start];
        for (int c = 0; c < numGlyphs; c++) {
            //int xOffset, yOffset;  // offset from glyph origin to top-left of bitmap
            int x0, y0, x1, y1;
            stbtt_GetCodepointBitmapBox(&fontInfo, c + start, font.scaleX, font.scaleY, &x0, &y0, &x1, &y1);
            glyphs[c].height = y1 - y0;
            glyphs[c].width = x1 - x0;
            glyphs[c].codePoint = c + start;
            stbtt_GetCodepointHMetrics(&fontInfo, c + start, &glyphs[c].advanceWidth, &glyphs[c].leftSideBearing);
        }

        // sort in size order to get a bit better packing by inserting in height order.
        // Chose height due to the fact that the tree has a property of causing an entire strip along the x axis to be constrained by the height
        // of the previous glyph.  This is due to the constraint of a glyph having to fit perfectly in a given rectangle where we split a parent
        // rectangle top to bottom first then left to right.  The top/bottom split means that any further division can fit no taller of a glyph.
        // Sorting by height means that any subsequent glyph will fit potentially right next to the previous.
        std::sort(glyphs, glyphs + numGlyphs,
            [](Glyph &a, Glyph &b) -> bool { return a.height > b.height; });
        packGlyphs(texture, texW, texH, glyphs, numGlyphs, &fontInfo, font.scaleX, font.scaleY);
        std::sort(glyphs, glyphs + numGlyphs,
            [](Glyph &a, Glyph &b) -> bool { return a.codePoint < b.codePoint; });


        for (int i = 0; i < numGlyphs; i++) {
            for (int j = 0; j < numGlyphs; j++) {
                int kernAdvance = stbtt_GetCodepointKernAdvance(&fontInfo, glyphs[i].codePoint, glyphs[j].codePoint);
                if (kernAdvance != 0) {
                    DBG_ASSERT(glyphs[i].codePoint < INT16_MAX && glyphs[i].codePoint > INT16_MIN, "code point out of bounds");
                    DBG_ASSERT(glyphs[j].codePoint < INT16_MAX && glyphs[i].codePoint > INT16_MIN, "code point out of bounds");

                    font.kernTable.put(getCodePointPairKey(glyphs[i].codePoint, glyphs[j].codePoint), kernAdvance);
                }
            }
        }



        DBG_ASSERT(stbi_write_png("out.png", texW, texH, 1, texture, texW) > 0);
        delete[] buffer;
    }

    int Text::getCodePointPairKey(int a, int b) {
        return (a << 16) | (b & 0xffff);
    }
}

