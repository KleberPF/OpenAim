#pragma once

#include "glm/glm.hpp"
#include "Shader.hpp"

#include <array>

class SpriteRenderer
{
public:
    SpriteRenderer(const Shader& shader);
    ~SpriteRenderer();

    void render(const glm::vec2& pos, float rotation,
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
    Shader shader_;
    GLuint vao_;
    GLuint vbo_;
};