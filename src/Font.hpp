#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include <array>
#include <memory>

constexpr size_t GLYPH_COUNT = 300;

struct Glyph {
    glm::ivec2 topLeft;
    glm::ivec2 size;
    glm::ivec2 bearing;
    long advance;
};

class Font {
public:
    Font(const char* path);

    static glm::vec2 toAtlasCoords(const glm::ivec2& coord);
    const Glyph& getGlyph(char c) const;
    void use() const;

    const Texture* texture() const;

private:
    std::array<Glyph, GLYPH_COUNT> m_glyphs;
    std::unique_ptr<Texture> m_texture;
};