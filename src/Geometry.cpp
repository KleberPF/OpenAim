#include "Geometry.hpp"

#include "utils.hpp"

#include <glm/gtc/matrix_transform.hpp>

Geometry::Geometry(const Model& model, Material& material, const Shader& shader,
    const glm::vec3& position)
    : m_model(&model)
    , m_material(&material)
    , m_shader(&shader)
    , m_position(position)
{
}

void Geometry::render() const
{
    m_model->render();
}

void Geometry::setPosition(const glm::vec3& position)
{
    m_position = position;

    updateMatrices();
}
void Geometry::setSize(const glm::vec3& size)
{
    m_size = size;

    updateMatrices();
}
void Geometry::setRotation(const glm::vec3& rotation)
{
    m_rotation = rotation;

    updateMatrices();
}

void Geometry::updateMatrices()
{
    // translation
    m_modelMatrix = glm::identity<glm::mat4>();
    m_modelMatrix = glm::translate(m_modelMatrix, m_position);

    // rotation
    if (m_rotation.has_value()) {
        m_modelMatrix *= anglesToRotationMatrix(m_rotation.value());
    }

    // scaling
    m_modelMatrix = glm::scale(m_modelMatrix, m_size);

    // update normal
    m_normalMatrix = glm::mat3(glm::transpose(glm::inverse(m_modelMatrix)));
}
