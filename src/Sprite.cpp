#include "Sprite.hpp"

#include "Shader.hpp"

Sprite::Sprite(std::vector<float> vertices,
               std::optional<std::vector<GLuint>> indices, glm::vec3 pos)
    : obj_(vertices, indices)
    , shader_("../resources/shaders/shader.vert",
              "../resources/shaders/shader.frag")
    , pos_(pos)
{
}

const Shader& Sprite::getShader() const
{
    return this->shader_;
}

const glm::vec3& Sprite::getPos() const
{
    return this->pos_;
}

void Sprite::render()
{
    this->obj_.render();
}
