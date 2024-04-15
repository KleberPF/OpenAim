#include "Renderer.hpp"

#include "Entity.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "utils.hpp"

Renderer::Renderer(const Window& window, const Camera& camera)
    : m_window(window)
    , m_camera(camera)
{
    // sprite
    glGenVertexArrays(1, &m_spriteVao);
    glGenBuffers(1, &m_spriteVbo);
    glBindVertexArray(m_spriteVao);

    glBindBuffer(GL_ARRAY_BUFFER, m_spriteVbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(m_spriteVertices[0]) * m_spriteVertices.size(),
        m_spriteVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        sizeof(m_spriteVertices[0]) * 2, (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // cubemap
    glGenVertexArrays(1, &m_skyboxVao);
    glGenBuffers(1, &m_skyboxVbo);
    glBindVertexArray(m_skyboxVao);

    glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(m_skyboxVertices[0]) * m_skyboxVertices.size(),
        m_skyboxVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(m_skyboxVertices[0]) * 3, (void*)nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_spriteVao);
    glDeleteBuffers(1, &m_spriteVbo);
    glDeleteVertexArrays(1, &m_skyboxVao);
    glDeleteBuffers(1, &m_skyboxVbo);
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
    if (entity.m_rotation.has_value()) {
        model *= anglesToRotationMatrix(entity.m_rotation.value());
    }

    // scaling
    model = glm::scale(model, entity.m_size);

    glm::mat4 projection
        = glm::perspective(glm::radians(m_camera.get().getZoom()),
            (float)m_window.get().getWidth() / m_window.get().getHeight(), 0.1F,
            100.0F);

    glm::mat4 mvp = projection * view * model;
    shader.setMat4("mvp", mvp);

    entity.m_model.getMaterial().bind(shader);
    entity.m_model.render();
}

void Renderer::renderSprite(const Shader& shader, const glm::vec2& pos,
    float rotation, const glm::vec2& dimensions, const glm::vec3& color)
{
    shader.use();
    shader.setVec3("color", color);

    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(pos, 1.0f));

    model = glm::translate(
        model, glm::vec3(0.5 * dimensions.x, 0.5 * dimensions.y, 0));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
    model = glm::translate(
        model, glm::vec3(-0.5 * dimensions.x, -0.5 * dimensions.y, 0));
    model = glm::scale(model, glm::vec3(dimensions, 1.0));

    glm::mat4 projection = glm::ortho(-m_window.get().getWidth() / 2.0f,
        m_window.get().getWidth() / 2.0f, -m_window.get().getHeight() / 2.0f,
        m_window.get().getHeight() / 2.0f, -1.0f, 1.0f);

    glm::mat4 mvp = projection * model;
    shader.setMat4("mvp", mvp);

    glBindVertexArray(m_spriteVao);
    glDrawArrays(GL_TRIANGLES, 0, m_spriteVertices.size() / 3);
}

void Renderer::renderSkybox(const Shader& shader, const Cubemap& cubemap)
{
    glDepthFunc(GL_LEQUAL);
    shader.use();
    // ... set view and projection matrix
    glm::mat4 view = glm::mat4(glm::mat3(m_camera.get().getViewMatrix()));
    shader.setMat4("view", view);

    glm::mat4 projection
        = glm::perspective(glm::radians(m_camera.get().getZoom()),
            (float)m_window.get().getWidth() / m_window.get().getHeight(), 0.1F,
            100.0F);
    shader.setMat4("projection", projection);

    glBindVertexArray(m_skyboxVao);
    glActiveTexture(GL_TEXTURE0); // temp
    cubemap.bind();
    glDrawArrays(GL_TRIANGLES, 0, m_skyboxVertices.size() / 3);
    glDepthFunc(GL_LESS);
}
