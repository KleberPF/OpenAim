#pragma once

#include "Material.hpp"
#include "Shader.hpp"
#include "glm/glm.hpp"
#include <functional>
#include <optional>

struct Sprite {
    Sprite(const Shader& shader, const Material& material,
        const glm::vec2& dimensions, const glm::vec2& position,
        float rotationAngle)
        : shader(shader)
        , material(material)
        , dimensions(dimensions)
        , position(position)
        , rotationAngle(rotationAngle)
    {
    }

    std::reference_wrapper<const Shader> shader;
    std::reference_wrapper<const Material> material;
    glm::vec2 dimensions;
    glm::vec2 position;
    float rotationAngle = 0.0f;
};