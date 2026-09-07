#pragma once

#include "Material.hpp"
#include "Model.hpp"

using Rotation = glm::vec3;

class HealthBar;
class Geometry {
public:
    Geometry(const Model& model, Material& material, const Shader& shader,
        const glm::vec3& pos);

    void render() const;

    void setPosition(const glm::vec3& position);
    void setSize(const glm::vec3& size);
    void setRotation(const glm::vec3& rotation);

    // Meant to be called after a move, resize or rotation
    void updateMatrices();

private:
    const Model* m_model;
    Material* m_material;
    const Shader* m_shader;

    glm::mat4 m_modelMatrix;
    glm::mat3 m_normalMatrix;

    glm::vec3 m_size;
    glm::vec3 m_position;
    std::optional<Rotation> m_rotation;

    friend class Renderer;
    friend class HealthBar;
};
