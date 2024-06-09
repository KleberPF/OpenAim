#include "Renderer.hpp"

#include "Entity.hpp"
#include "Material.hpp"
#include "Shader.hpp"

#include <glm/gtc/matrix_transform.hpp>

Renderer::Renderer()
{
    // sprite
    glGenVertexArrays(1, &m_spriteVao);
    glGenBuffers(1, &m_spriteVbo);

    glBindBuffer(GL_ARRAY_BUFFER, m_spriteVbo);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(m_spriteVertices[0]) * m_spriteVertices.size(),
        m_spriteVertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(m_spriteVao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)nullptr);
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

void Renderer::renderEntity(const Scene& scene, const Entity& entity)
{
    const Shader& shader = entity.shader.get();
    shader.use();

    // lighting stuff
    if (scene.globalLightSource.has_value()) {
        shader.setVec3("viewPos", scene.camera.position);
        shader.setVec3(
            "light.direction", scene.globalLightSource->get().direction);
        shader.setVec3("light.ambient", scene.globalLightSource->get().ambient);
        shader.setVec3("light.diffuse", scene.globalLightSource->get().diffuse);
        shader.setVec3(
            "light.specular", scene.globalLightSource->get().specular);
    }

    glm::mat4 view = scene.camera.buildViewMatrix();
    shader.setMat4("view", view);

    auto model = entity.modelMatrix();
    shader.setMat4("model", model);
    shader.setMat3("normal", entity.normalMatrix());

    glm::mat4 projection = glm::perspective(glm::radians(scene.camera.zoom()),
        (float)scene.viewportWidth / scene.viewportHeight, 0.1F, 100.0F);

    glm::mat4 mvp = projection * view * model;
    shader.setMat4("mvp", mvp);

    entity.material.get().bind(shader);
    entity.render();

    if (entity.shouldRenderHealthBar()) {
        const Shader& healthbarShader = entity.healthbarShader;
        Material& healthbarMaterial = entity.healthbarMaterial;

        healthbarShader.use();

        model = entity.buildHealthbarModelMatrix();
        mvp = projection * view * model;

        healthbarShader.setMat4("mvp", mvp);
        healthbarShader.setFloat(
            "healthPercentage", entity.getHealthPercentage());
        healthbarMaterial.setColor(entity.getHealthBarColor());
        healthbarMaterial.bind(healthbarShader);
        entity.renderHealthBar();
    }
}

void Renderer::renderSprite(const Scene& scene, const Sprite& sprite) const
{
    sprite.shader.get().use();
    sprite.material.get().bind(sprite.shader);

    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(sprite.position, 1.0f));

    if (sprite.rotationAngle != 0.0f) {
        model = glm::translate(model,
            glm::vec3(0.5 * sprite.dimensions.x, 0.5 * sprite.dimensions.y, 0));
        model = glm::rotate(
            model, glm::radians(sprite.rotationAngle), glm::vec3(0, 0, 1));
        model = glm::translate(model,
            glm::vec3(
                -0.5 * sprite.dimensions.x, -0.5 * sprite.dimensions.y, 0));
    }

    model = glm::scale(model, glm::vec3(sprite.dimensions, 1.0));

    glm::mat4 projection = glm::ortho(-scene.viewportWidth / 2.0f,
        scene.viewportWidth / 2.0f, -scene.viewportHeight / 2.0f,
        scene.viewportHeight / 2.0f, -1.0f, 1.0f);

    glm::mat4 mvp = projection * model;
    sprite.shader.get().setMat4("mvp", mvp);

    glBindVertexArray(m_spriteVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::renderSkybox(
    const Scene& scene, const Shader& shader, const Cubemap& cubemap) const
{
    glDepthFunc(GL_LEQUAL);
    shader.use();
    // ... set view and projection matrix
    glm::mat4 view = glm::mat4(glm::mat3(scene.camera.buildViewMatrix()));
    shader.setMat4("view", view);

    glm::mat4 projection = glm::perspective(glm::radians(scene.camera.zoom()),
        (float)scene.viewportWidth / scene.viewportHeight, 0.1F, 100.0F);
    shader.setMat4("projection", projection);

    glBindVertexArray(m_skyboxVao);
    glActiveTexture(GL_TEXTURE0); // temp
    cubemap.bind();
    glDrawArrays(GL_TRIANGLES, 0, m_skyboxVertices.size() / 3);
    glDepthFunc(GL_LESS);
}

void Renderer::renderScene(const Scene& scene)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (scene.entities.has_value()) {
        for (auto& entity : scene.entities->get()) {
            renderEntity(scene, entity);
        }
    }

    if (scene.skybox.has_value()) {
        renderSkybox(
            scene, scene.skybox->get().shader, scene.skybox->get().cubemap);
    }

    if (scene.sprites.has_value()) {
        for (auto& sprite : scene.sprites->get()) {
            renderSprite(scene, sprite);
        }
    }
}
