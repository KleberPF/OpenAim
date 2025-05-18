#include "Text.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Text::Text(const Font* font, const char* text = "")
    : m_contents(text)
    , m_font(font)
{
    setText(text);
}

void Text::setFont(const Font* font)
{
    m_font = font;
}

void Text::setText(const std::string& text)
{
    m_width = 0;

    for (auto c : text) {
        const Glyph& g = m_font->getGlyph(c);
        m_height = std::max<float>(g.size.y, m_height);
        m_width += g.advance;
    }
}

const std::string& Text::contents() const
{
    return m_contents;
}

int Text::width() const
{
    return m_width;
}

int Text::height() const
{
    return m_height;
}
