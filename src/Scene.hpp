#pragma once

#include "Camera.hpp"
#include "Entity.hpp"
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
    Scene(const Camera& camera)
        : camera(camera)
    {
    }

    int viewportWidth;
    int viewportHeight;
    const Camera& camera;
    LightSource* globalLightSource = nullptr;
    Skybox* skybox = nullptr;
    std::vector<Entity>* entities = nullptr;
    std::vector<Sprite>* sprites = nullptr;
};