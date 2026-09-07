#pragma once

#include "Camera.hpp"
#include "Entity.hpp"
#include "Geometry.hpp"
#include "Material.hpp"
#include "Sprite.hpp"

#include <glm/glm.hpp>

#include <optional>
#include <vector>

struct LightSource {
    // A global light source doesn't have a position,
    // only a direction
    std::optional<glm::vec3> position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct Skybox {
    Skybox(const Cubemap& cubemap, const Shader& shader)
        : cubemap(cubemap)
        , shader(shader)
    {
    }

    const Cubemap& cubemap;
    const Shader& shader;
};

struct Scene {
    Scene(const Camera& camera, int viewportWidth, int viewportHeight)
        : camera(camera)
        , viewportWidth(viewportWidth)
        , viewportHeight(viewportHeight)
    {
    }

    const Camera& camera;
    int viewportWidth;
    int viewportHeight;
    const LightSource* globalLightSource = nullptr;
    const Skybox* skybox = nullptr;
    std::vector<Entity>* entities = nullptr;
    const std::vector<Sprite>* sprites = nullptr;
    const std::vector<Geometry>* geometries = nullptr;
};
