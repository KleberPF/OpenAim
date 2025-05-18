#pragma once

#include "Material.hpp"

#include <ft2build.h>
// freetype.h has to be included after
#include <freetype/freetype.h>
#include <glm/glm.hpp>

#include <array>
#include <map>
#include <memory>

constexpr size_t GLYPH_COUNT = 300;
constexpr unsigned int MAX_FONT_SIZE = 48;

struct Glyph {
    glm::ivec2 topLeft;
    glm::ivec2 size;
    glm::ivec2 bearing;
    long advance;
};

struct FontSizeData {
    std::array<Glyph, GLYPH_COUNT> glyphs;
    std::unique_ptr<Texture> texture;
};

class Font {
public:
    Font(const char* path);

    static glm::vec2 toAtlasCoords(const glm::ivec2& coord);
    const Glyph& getGlyph(char c, int fontSize) const;

    const Texture* texture(int fontSize) const;
    Font& generateGlyphsForFontSize(int fontSize);

private:
    FT_Library m_ft;
    FT_Face m_face;
    std::map<int, FontSizeData> m_glyphMap;
};