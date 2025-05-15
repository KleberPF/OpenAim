#include "Font.hpp"

#include "Material.hpp"

#include <ft2build.h>
// freetype.h has to be included after
#include <freetype/freetype.h>

#include <array>
#include <memory>

constexpr size_t ATLAS_SIZE = 512; // pixels
constexpr int GLYPHS_PER_LINE = 20;

Font::Font(const char* path)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        exit(-1);
    }

    FT_Face face;
    if (FT_New_Face(ft, path, 0, &face)) {
        exit(-1);
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    int xoffset = 0;
    int yoffset = 0;
    std::array<unsigned char, ATLAS_SIZE * ATLAS_SIZE> atlas;

    for (int i = 0; i < 95; i++) {
        unsigned char c = ' ' + i;

        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            continue;
        }

        if (i != 0 && i % GLYPHS_PER_LINE == 0) {
            yoffset += ((face->size->metrics.height >> 6) + 1);
            xoffset = 0;
        }

        FT_Bitmap* bmp = &face->glyph->bitmap;

        for (unsigned int row = 0; row < bmp->rows; row++) {
            for (unsigned int col = 0; col < bmp->width; col++) {
                int x = xoffset + col;
                int y = yoffset + row;
                // fill atlas backwards because OpenGL textures are inverted in memory
                atlas[ATLAS_SIZE * (ATLAS_SIZE - 1 - y) + x] = bmp->buffer[row * bmp->pitch + col];
            }
        }

        m_glyphs[c] = {
            .topLeft = { xoffset, yoffset },
            .size = { bmp->width, bmp->rows },
            .bearing = { face->glyph->bitmap_left, face->glyph->bitmap_top },
            .advance = face->glyph->advance.x >> 6
        };

        xoffset += bmp->width + 1;
    }

    m_texture = std::make_unique<Texture>(GL_RED, ATLAS_SIZE, ATLAS_SIZE, atlas.data(), Texture::Type::Diffuse);
}

glm::vec2 Font::toAtlasCoords(const glm::ivec2& coord)
{
    return { (float)coord.x / ATLAS_SIZE, 1 - (float)coord.y / ATLAS_SIZE };
}

const Glyph& Font::getGlyph(char c) const
{
    return m_glyphs[c];
}

void Font::use() const
{
    m_texture->bind();
}

const Texture* Font::texture() const
{
    return m_texture.get();
}
