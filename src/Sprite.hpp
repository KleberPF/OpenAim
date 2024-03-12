#pragma once

#include "GLObject.hpp"
#include "Shader.hpp"

class Sprite
{
public:
    Sprite(std::vector<float> vertices,
           std::optional<std::vector<GLuint>> indices, glm::vec3 pos);

    const Shader& getShader() const;
    const glm::vec3& getPos() const;

    void render();

private:
    GLObject obj_;
    Shader shader_;
    glm::vec3 pos_;
};