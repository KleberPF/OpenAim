#include "TextRenderable.hpp"

TextRenderable::TextRenderable(Text text)
    : m_text(std::move(text))
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    createRenderData();
}

const Text& TextRenderable::text() const
{
    return m_text;
}

void TextRenderable::setText(Text text)
{
    m_text = std::move(text);
    createRenderData();
}

RenderData TextRenderable::getRenderData() const
{
    return {
        .vao = m_vao,
        .vertexCount = m_vertices.size(),
        .texture = m_text.m_font->texture()
    };
}

void TextRenderable::createRenderData()
{
    m_vertices.clear();
    int width = 0;

    for (auto c : m_text.contents()) {
        const Glyph& g = m_text.m_font->getGlyph(c);

        float xpos = width + g.bearing.x;
        float ypos = g.bearing.y - g.size.y;

        float w = g.size.x;
        float h = g.size.y;

        glm::vec2 texBottomLeft = Font::toAtlasCoords({ g.topLeft.x, g.topLeft.y + g.size.y });
        glm::vec2 texTopRight = Font::toAtlasCoords({ g.topLeft.x + g.size.x, g.topLeft.y });

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

        width += g.advance;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(internal::Vertex), m_vertices.data(), GL_DYNAMIC_DRAW);
}
