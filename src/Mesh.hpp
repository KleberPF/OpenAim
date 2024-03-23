#pragma once

#include "Shader.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>
#include <string>
#include <vector>

constexpr auto MAX_BONE_INFLUENCE = 4;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    std::array<int, MAX_BONE_INFLUENCE> boneIds;
    std::array<float, MAX_BONE_INFLUENCE> weights;
};

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;
    GLuint vao;

    Mesh(std::vector<Vertex>& vertices, std::vector<GLuint>& indices,
         std::vector<Texture>& textures);

    void render(const Shader& shader);

private:
    GLuint vbo, ebo;
};
