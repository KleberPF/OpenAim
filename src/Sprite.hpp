#pragma once

#include "Material.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>
#include <functional>

// Kinda ugly
struct SpritePosition {
    float x;
    float y;
    bool relative = false;
};

struct SpriteDimensions {
    float width;
    float height;
    bool relative = false;
};

// TODO: Kinda messy, maybe use a builder pattern or something here
struct Sprite {
    Sprite(const Shader& shader, const Material& material,
        const SpriteDimensions& dimensions, const SpritePosition& position,
        float rotationAngle, const glm::vec2& pivot)
        : shader(shader)
        , material(material)
        , dimensions(dimensions)
        , position(position)
        , rotationAngle(rotationAngle)
        , pivot(pivot)
    {
    }

    std::reference_wrapper<const Shader> shader;
    std::reference_wrapper<const Material> material;
    SpriteDimensions dimensions;
    SpritePosition position;
    float rotationAngle = 0.0f;

    // Pivot point, where (0, 0) is top left and (1, 1) is bottom right
    // Also supports everything in between
    glm::vec2 pivot;
};
