#pragma once

#include "Color.hpp"
#include "Material.hpp"
#include "Scene.hpp"
#include "Shader.hpp"
#include "Sprite.hpp"
#include "TextRenderable.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <array>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void renderScene(const Scene& scene);
    void renderText(const TextRenderable& renderable, float x, float y) const;

    void renderRectangle(float x, float y, float width, float height, const Color& color) const;

    // TODO: temporary approach
    glm::mat4 orthoProjection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f);

private:
    // Should probably change this later, having to always pass the scene
    // is kinda ugly
    static void renderEntity(const Scene& scene, const Entity& entity);
    void renderSprite(const Scene& scene, const Sprite& sprite) const;
    void renderSkybox(
        const Scene& scene, const Shader& shader, const Cubemap& cubemap) const;

    // clang-format off
    std::array<float, 24> m_rectangleVertices = { 
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 
    
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    std::array<float, 108> m_skyboxVertices = {      
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
    };
    // clang-format on

    GLuint m_rectangleVao;
    GLuint m_rectangleVbo;
    GLuint m_skyboxVao;
    GLuint m_skyboxVbo;
};
