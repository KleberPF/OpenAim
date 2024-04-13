#pragma once

#include "Camera.hpp"
#include "Shader.hpp"
#include "Window.hpp"

#include <array>
#include <functional>

class Entity;

class Renderer
{
public:
    Renderer(const Window& window, const Camera& camera,
             const Shader& spriteShader);
    ~Renderer();

    void renderEntity(const Entity& entity);
    void renderSprite(const glm::vec2& pos, float rotation,
                      const glm::vec2& dimensions, const glm::vec3& color);

private:
    // clang-format off
    std::array<float, 18> vertices_ = {
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
    std::reference_wrapper<const Window> window_;
    std::reference_wrapper<const Camera> camera_;
    std::reference_wrapper<const Shader> spriteShader_;
    GLuint vao_;
    GLuint vbo_;
};