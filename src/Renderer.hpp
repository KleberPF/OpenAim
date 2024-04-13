#pragma once

#include "Camera.hpp"
#include "Shader.hpp"
#include "Window.hpp"

#include <array>
#include <functional>

class Entity;

class Renderer {
public:
    Renderer(const Window& window, const Camera& camera,
        const Shader& spriteShader);
    ~Renderer();

    void renderEntity(const Entity& entity);
    void renderSprite(const glm::vec2& pos, float rotation,
        const glm::vec2& dimensions, const glm::vec3& color);

private:
    // clang-format off
    std::array<float, 18> m_vertices = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0,
    };
    // clang-format on

    // Don't really like having a window here, but it will
    // do for now
    std::reference_wrapper<const Window> m_window;
    std::reference_wrapper<const Camera> m_camera;
    std::reference_wrapper<const Shader> m_spriteShader;
    GLuint m_vao;
    GLuint m_vbo;
};