#pragma once

#include "Camera.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "Window.hpp"

#include <array>
#include <functional>

class Entity;

class Renderer {
public:
    Renderer(const Window& window, const Camera& camera);
    ~Renderer();

    void renderEntity(const Entity& entity);
    void renderSprite(const Shader& shader, const glm::vec2& pos, float rotation,
        const glm::vec2& dimensions, const glm::vec3& color);
    void renderSkybox(const Shader& shader, const Cubemap& cubemap);

private:
    // clang-format off
    std::array<float, 18> m_spriteVertices = {
        0, 0,
        1, 0,
        0, 1,
        1, 0,
        1, 1,
        0, 1,
    };

    std::array<float, 108> m_skyboxVertices = {      
        -1,  1, -1,
        -1, -1, -1,
         1, -1, -1,
         1, -1, -1,
         1,  1, -1,
        -1,  1, -1,

        -1, -1,  1,
        -1, -1, -1,
        -1,  1, -1,
        -1,  1, -1,
        -1,  1,  1,
        -1, -1,  1,

         1, -1, -1,
         1, -1,  1,
         1,  1,  1,
         1,  1,  1,
         1,  1, -1,
         1, -1, -1,

        -1, -1,  1,
        -1,  1,  1,
         1,  1,  1,
         1,  1,  1,
         1, -1,  1,
        -1, -1,  1,

        -1,  1, -1,
         1,  1, -1,
         1,  1,  1,
         1,  1,  1,
        -1,  1,  1,
        -1,  1, -1,

        -1, -1, -1,
        -1, -1,  1,
         1, -1, -1,
         1, -1, -1,
        -1, -1,  1,
         1, -1,  1
    };
    // clang-format on

    // Don't really like having a window here, but it will
    // do for now
    std::reference_wrapper<const Window> m_window;
    std::reference_wrapper<const Camera> m_camera;

    GLuint m_spriteVao;
    GLuint m_spriteVbo;
    GLuint m_skyboxVao;
    GLuint m_skyboxVbo;
};