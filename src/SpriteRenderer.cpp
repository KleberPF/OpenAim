#include "SpriteRenderer.hpp"

#include "glm/ext/matrix_transform.hpp"
#include "Shader.hpp"

SpriteRenderer::SpriteRenderer(const Shader& shader)
    : shader_(shader)
{
    glGenVertexArrays(1, &this->vao_);
    glGenBuffers(1, &this->vbo_);
    glBindVertexArray(this->vao_);

    glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(this->vertices_[0]) * this->vertices_.size(),
                 this->vertices_.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(this->vertices_[0]) * 2, (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &this->vao_);
    glDeleteBuffers(1, &this->vbo_);
}

void SpriteRenderer::render(const glm::vec2& pos, float rotation,
                            const glm::vec2& dimensions, const glm::vec3& color)
{
    // this->shader_.use();

    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(pos, 1.0f));

    model = glm::translate(
        model, glm::vec3(0.5 * dimensions.x, 0.5 * dimensions.y, 0));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
    model = glm::translate(
        model, glm::vec3(-0.5 * dimensions.x, -0.5 * dimensions.y, 0));
    model = glm::scale(model, glm::vec3(dimensions, 1.0));
    this->shader_.setMat4("model", model);

    this->shader_.setVec3("color", color);

    glBindVertexArray(this->vao_);
    glDrawArrays(GL_TRIANGLES, 0, this->vertices_.size() / 3);
}