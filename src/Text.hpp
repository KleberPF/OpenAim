#pragma once

#include "Color.hpp"
#include "Font.hpp"

#include <string>

struct RenderData {
    GLuint vao;
    size_t vertexCount;
    const Texture* texture;
};

namespace internal {

struct Vertex {
    glm::vec2 position;
    glm::vec2 texCoords;
};

} // namespace internal

class Text {
public:
    Text(const Font& font, const char* text);

    void setFont(const Font& font);
    void setText(const std::string& text);
    RenderData getRenderData() const;

    const std::string& contents() const;
    int width() const;
    int height() const;

    Color color = { .r = 255, .g = 255, .b = 255 };

private:
    void recreateVBO();

    // this is the width in pixels of the text
    // scale is not being account for here
    int m_width = 0;
    // this is the height of the tallest letter
    int m_height = 0;
    std::string m_contents;
    std::reference_wrapper<const Font> m_font;
    std::vector<internal::Vertex> m_vertices;

    // GL stuff
    GLuint m_vao;
    GLuint m_vbo;
};