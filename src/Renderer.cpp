#include "Renderer.hpp"

#include "Entity.hpp"
#include "Shader.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

Renderer::Renderer(const Window& window, const Camera& camera,
    const Shader& spriteShader)
    : m_window(window)
    , m_camera(camera)
    , m_spriteShader(spriteShader)
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(m_vertices[0]) * m_vertices.size(),
        m_vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        sizeof(m_vertices[0]) * 2, (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

void Renderer::renderEntity(const Entity& entity)
{
    const Shader& shader = entity.m_model.getShader();
    shader.use();

    shader.setFloat("textureScale", 16);
    shader.setVec3("color", entity.m_color);

    glm::mat4 view = m_camera.get().getViewMatrix();
    shader.setMat4("view", view);

    // translation
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, entity.m_currentPos);

    // rotation
    model = glm::rotate(model, glm::radians(entity.m_rotation.angle),
        entity.m_rotation.axis);

    // scaling
    model = glm::scale(model, entity.m_size);

    glm::mat4 projection = glm::perspective(
        glm::radians(m_camera.get().getZoom()),
        (float)m_window.get().getWidth() / m_window.get().getHeight(), 0.1F,
        100.0F);

    glm::mat4 mvp = projection * view * model;
    shader.setMat4("mvp", mvp);

    entity.m_model.getMaterial().bind(shader);
    entity.m_model.render();
}

void Renderer::renderSprite(const glm::vec2& pos, float rotation,
    const glm::vec2& dimensions, const glm::vec3& color)
{
    m_spriteShader.get().use();
    m_spriteShader.get().setVec3("color", color);

    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(pos, 1.0f));

    model = glm::translate(
        model, glm::vec3(0.5 * dimensions.x, 0.5 * dimensions.y, 0));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
    model = glm::translate(
        model, glm::vec3(-0.5 * dimensions.x, -0.5 * dimensions.y, 0));
    model = glm::scale(model, glm::vec3(dimensions, 1.0));

    glm::mat4 projection = glm::ortho(
        -m_window.get().getWidth() / 2.0f, m_window.get().getWidth() / 2.0f,
        -m_window.get().getHeight() / 2.0f, m_window.get().getHeight() / 2.0f,
        -1.0f, 1.0f);

    glm::mat4 mvp = projection * model;
    m_spriteShader.get().setMat4("mvp", mvp);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3);
}