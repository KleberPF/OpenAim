#include "Renderer.hpp"

#include "Entity.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "Shader.hpp"

Renderer::Renderer(const Window& window, const Camera& camera,
                   const Shader& spriteShader)
    : window_(window)
    , camera_(camera)
    , spriteShader_(spriteShader)
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

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &this->vao_);
    glDeleteBuffers(1, &this->vbo_);
}

void Renderer::renderEntity(const Entity& entity)
{
    const Shader& shader = entity.model_.getShader();
    shader.use();

    shader.setFloat("textureScale", 16);
    shader.setVec3("color", entity.color);

    glm::mat4 view = this->camera_.get().getViewMatrix();
    shader.setMat4("view", view);

    // translation
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, entity.currentPos_);

    // rotation
    model = glm::rotate(model, glm::radians(entity.rotation_.angle),
                        entity.rotation_.axis);

    // scaling
    model = glm::scale(model, entity.size);

    glm::mat4 projection = glm::perspective(
        glm::radians(this->camera_.get().getZoom()),
        (float)this->window_.get().width / this->window_.get().height, 0.1F,
        100.0F);

    glm::mat4 mvp = projection * view * model;
    shader.setMat4("mvp", mvp);

    entity.model_.getMaterial().bind(shader);
    entity.model_.render();
}

void Renderer::renderSprite(const glm::vec2& pos, float rotation,
                            const glm::vec2& dimensions, const glm::vec3& color)
{
    this->spriteShader_.get().use();
    this->spriteShader_.get().setVec3("color", color);

    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(pos, 1.0f));

    model = glm::translate(
        model, glm::vec3(0.5 * dimensions.x, 0.5 * dimensions.y, 0));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
    model = glm::translate(
        model, glm::vec3(-0.5 * dimensions.x, -0.5 * dimensions.y, 0));
    model = glm::scale(model, glm::vec3(dimensions, 1.0));

    glm::mat4 projection = glm::ortho(
        -this->window_.get().width / 2.0f, this->window_.get().width / 2.0f,
        -this->window_.get().height / 2.0f, this->window_.get().height / 2.0f,
        -1.0f, 1.0f);

    glm::mat4 mvp = projection * model;
    this->spriteShader_.get().setMat4("mvp", mvp);

    glBindVertexArray(this->vao_);
    glDrawArrays(GL_TRIANGLES, 0, this->vertices_.size() / 3);
}