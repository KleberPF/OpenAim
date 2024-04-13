#pragma once

#include "Shader.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>
#include <string>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);

    void render() const;

private:
    GLuint m_vbo;
    GLuint m_ebo;

    std::vector<Vertex> m_vertices;
    std::vector<GLuint> m_indices;
    GLuint m_vao;
};
