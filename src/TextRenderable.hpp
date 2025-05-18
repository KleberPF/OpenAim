#pragma once

#include "Text.hpp"

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

class TextRenderable {
public:
    TextRenderable(Text text);

    const Text& text() const;
    void setText(Text text);

    RenderData getRenderData() const;

private:
    void createRenderData();

    Text m_text;
    std::vector<internal::Vertex> m_vertices;

    // GL stuff
    GLuint m_vao;
    GLuint m_vbo;
};
