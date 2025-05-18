#include "Font.hpp"

#include "Material.hpp"

// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include <stb_image_write.h>

#include <array>

constexpr size_t ATLAS_SIZE = 512; // pixels
constexpr int GLYPHS_PER_LINE = 15;

Font::Font(const char* path)
{
    if (FT_Init_FreeType(&m_ft)) {
        exit(-1);
    }

    if (FT_New_Face(m_ft, path, 0, &m_face)) {
        exit(-1);
    }

    generateGlyphsForFontSize(MAX_FONT_SIZE);
}

glm::vec2 Font::toAtlasCoords(const glm::ivec2& coord)
{
    return { (float)coord.x / ATLAS_SIZE, 1 - (float)coord.y / ATLAS_SIZE };
}

const Glyph& Font::getGlyph(char c, int fontSize) const
{
    return m_glyphMap.at(fontSize).glyphs[c];
}

const Texture* Font::texture(int fontSize) const
{
    return m_glyphMap.at(fontSize).texture.get();
}

Font& Font::generateGlyphsForFontSize(int fontSize)
{
    if (m_glyphMap.contains(fontSize)) {
        return *this;
    }

    FontSizeData& data = m_glyphMap[fontSize];

    FT_Set_Pixel_Sizes(m_face, 0, fontSize);

    int xoffset = 0;
    int yoffset = 0;
    std::array<unsigned char, ATLAS_SIZE * ATLAS_SIZE> atlas = {};

    for (int i = 0; i < 95; i++) {
        unsigned char c = ' ' + i;

        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
            continue;
        }

        if (i != 0 && i % GLYPHS_PER_LINE == 0) {
            yoffset += ((m_face->size->metrics.height >> 6) + 1);
            xoffset = 0;
        }

        FT_Bitmap* bmp = &m_face->glyph->bitmap;

        for (unsigned int row = 0; row < bmp->rows; row++) {
            for (unsigned int col = 0; col < bmp->width; col++) {
                int x = xoffset + col;
                int y = yoffset + row;
                // fill atlas backwards because OpenGL textures are inverted vertically in memory
                // basically the (x, y) = y * Width + x formula but backwards
                atlas[ATLAS_SIZE * (ATLAS_SIZE - 1 - y) + x] = bmp->buffer[row * bmp->pitch + col];
            }
        }

        data.glyphs[c] = {
            .topLeft = { xoffset, yoffset },
            .size = { bmp->width, bmp->rows },
            .bearing = { m_face->glyph->bitmap_left, m_face->glyph->bitmap_top },
            .advance = m_face->glyph->advance.x >> 6
        };

        xoffset += bmp->width + 1;
    }

    data.texture = std::make_unique<Texture>(GL_RED, ATLAS_SIZE, ATLAS_SIZE, atlas.data(), Texture::Type::Diffuse);
    // stbi_write_png("output.png", ATLAS_SIZE, ATLAS_SIZE, 1, atlas.data(), ATLAS_SIZE);

    return *this;
}
