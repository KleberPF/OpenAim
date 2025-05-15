#include "Text.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Text::Text(const Font& font, const char* text = "")
    : m_contents(text)
    , m_font(font)
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    setText(text);
}

void Text::setFont(const Font& font)
{
    m_font = font;
}

void Text::setText(const std::string& text)
{
    m_vertices.clear();
    m_width = 0;

    for (auto c : text) {
        const Glyph& g = m_font.get().getGlyph(c);

        float xpos = m_width + g.bearing.x;
        float ypos = g.bearing.y - g.size.y;

        float w = g.size.x;
        float h = g.size.y;

        m_height = std::max<float>(h, m_height);

        glm::vec2 texBottomLeft
            = Font::toAtlasCoords({ g.topLeft.x, g.topLeft.y + g.size.y });
        glm::vec2 texTopRight
            = Font::toAtlasCoords({ g.topLeft.x + g.size.x, g.topLeft.y });

        // clang-format off
        m_vertices.insert(m_vertices.end(), {
            {.position = {xpos    , ypos    }, .texCoords = {texBottomLeft.x, texBottomLeft.y}},
            {.position = {xpos + w, ypos    }, .texCoords = {texTopRight.x  , texBottomLeft.y}},
            {.position = {xpos    , ypos + h}, .texCoords = {texBottomLeft.x, texTopRight.y  }},
            {.position = {xpos + w, ypos    }, .texCoords = {texTopRight.x  , texBottomLeft.y}},
            {.position = {xpos + w, ypos + h}, .texCoords = {texTopRight.x  , texTopRight.y  }},
            {.position = {xpos    , ypos + h}, .texCoords = {texBottomLeft.x, texTopRight.y  }},
        });
        // clang-format on

        m_width += g.advance;
    }

    recreateVBO();
}

RenderData Text::getRenderData() const
{
    return {
        .vao = m_vao,
        .vertexCount = m_vertices.size(),
        .texture = m_font.get().texture()
    };
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

void Text::recreateVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(internal::Vertex), m_vertices.data(), GL_DYNAMIC_DRAW);
}
