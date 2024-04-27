#include "Entity.hpp"

#include "Model.hpp"
#include "Shader.hpp"
#include "utils.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <utility>

CollisionObject::CollisionObject(const glm::vec3& pos)
    : m_pos(pos)
{
}

void CollisionObject::setRotation(const Rotation& rotation)
{
    m_rotation = rotation;
}

CollisionBox::CollisionBox(const glm::vec3& pos, const glm::vec3& size)
    : CollisionObject(pos)
{
    setSize(size);
}

IntersectionResult CollisionBox::isIntersectedByLine(
    glm::vec3 eyePos, glm::vec3 eyeDir) const
{
    IntersectionResult result = std::nullopt;
    float closestDist = FLT_MAX;

    if (m_rotation.has_value()) {
        glm::mat4 moveToOrigin
            = glm::translate(glm::identity<glm::mat4>(), -m_pos);
        glm::mat4 inverseRotation
            = glm::inverse(anglesToRotationMatrix(m_rotation.value()));
        glm::mat4 moveBack = glm::translate(glm::identity<glm::mat4>(), m_pos);

        auto tmp = glm::vec4(eyePos, 1.0f);
        eyePos = glm::vec3(moveBack * inverseRotation * moveToOrigin * tmp);

        tmp = glm::vec4(eyeDir, 1.0f);
        eyeDir = glm::vec3(inverseRotation * tmp);
    }

    for (const auto& vertex : m_aabb) {
        for (size_t j = 0; j < 3; j++) {
            float t = (vertex[j] - eyePos[j]) / eyeDir[j];
            if (t < 0) {
                continue;
            }

            glm::vec3 intersection = eyePos + eyeDir * t;
            bool intersectsNow = isPointInPlaneSection(intersection);

            if (!intersectsNow) {
                continue;
            }

            float dist = glm::distance(intersection, eyePos);
            if (dist < closestDist) {
                closestDist = dist;
                result = { intersection, closestDist };
            }
        }
    }

    return result;
}

void CollisionBox::setSize(const glm::vec3& size)
{
    m_size = size;
    m_aabb[0] = m_pos - glm::vec3(0.5, 0.5, 0.5) * m_size;
    m_aabb[1] = m_pos + glm::vec3(0.5, 0.5, 0.5) * m_size;
}

void CollisionBox::move(const glm::vec3& newPos)
{
    m_pos = newPos;
    m_aabb[0] = m_pos - glm::vec3(0.5, 0.5, 0.5) * m_size;
    m_aabb[1] = m_pos + glm::vec3(0.5, 0.5, 0.5) * m_size;
}

bool CollisionBox::isPointInPlaneSection(const glm::vec3& point) const
{
    return (point.x <= m_aabb[1].x && point.x >= m_aabb[0].x
        && point.y <= m_aabb[1].y && point.y >= m_aabb[0].y
        && point.z <= m_aabb[1].z && point.z >= m_aabb[0].z);
}

CollisionSphere::CollisionSphere(const glm::vec3& pos, float radius)
    : CollisionObject(pos)
    , m_radius(radius)
{
}

void CollisionSphere::move(const glm::vec3& newPos)
{
    m_pos = newPos;
}

// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
IntersectionResult CollisionSphere::isIntersectedByLine(
    glm::vec3 eyePos, glm::vec3 eyeDir) const
{
    float delta = pow(glm::dot(eyeDir, eyePos - m_pos), 2)
        - (pow(glm::length(eyePos - m_pos), 2) - pow(m_radius, 2));

    if (delta < 0) {
        return std::nullopt;
    }

    float d = -(glm::dot(eyeDir, eyePos - m_pos)) - std::sqrt(delta);

    if (d > 0) {
        return { { eyePos + eyeDir * d, d } };
    }

    return std::nullopt;
}

void CollisionSphere::setSize(const glm::vec3& size)
{
    m_radius = size.x;
}

Entity::Entity(Model model, const glm::vec3& pos)
    : referentialPos(pos)
    , m_currentPos(pos)
    , m_model(std::move(model))
{
}

glm::mat4 Entity::modelMatrix() const
{
    return m_modelMatrix;
}

glm::mat3 Entity::normalMatrix() const
{
    return m_normalMatrix;
}

const Shader& Entity::shader() const
{
    return m_model.shader;
}

const Material& Entity::material() const
{
    return m_model.material;
}

void Entity::setRotation(float x, float y, float z)
{
    m_rotation = glm::vec3(x, y, z);
    if (m_collisionObject != nullptr) {
        m_collisionObject->setRotation(m_rotation.value());
    }

    updateMatrices();
}

void Entity::move(const glm::vec3& newPos)
{
    m_currentPos = newPos;
    if (m_collisionObject != nullptr) {
        m_collisionObject->move(newPos);
    }

    updateMatrices();
}

void Entity::moveRelative(const glm::vec3& newPos)
{
    move(referentialPos + newPos);
}

void Entity::addCollisionObject(CollisionObjectType type)
{
    switch (type) {
    case CollisionObjectType::SPHERE:
        m_collisionObject
            = std::make_unique<CollisionSphere>(m_currentPos, m_size.x);
        break;
    default:
        m_collisionObject
            = std::make_unique<CollisionBox>(m_currentPos, m_size);
        break;
    }

    if (m_rotation.has_value()) {
        m_collisionObject->setRotation(m_rotation.value());
    }
}

const CollisionObject* Entity::collisionObject() const
{
    return m_collisionObject.get();
}

void Entity::setSize(const glm::vec3& size)
{
    m_size = size;
    if (m_collisionObject != nullptr) {
        m_collisionObject->setSize(size);
    }

    updateMatrices();
}

const std::string& Entity::getName() const
{
    return m_name;
}

void Entity::setName(const std::string& name)
{
    m_name = name;
}

void Entity::setMovementPattern(std::function<glm::vec3(float)> callback)
{
    m_calculateNewPos = std::move(callback);
}

void Entity::update(float timePassedSeconds)
{
    if (!m_calculateNewPos) {
        return;
    }

    glm::vec3 newPos = m_calculateNewPos(timePassedSeconds);
    moveRelative(newPos);
}

void Entity::render() const
{
    m_model.render();
}

void Entity::updateMatrices()
{
    // translation
    m_modelMatrix = glm::identity<glm::mat4>();
    m_modelMatrix = glm::translate(m_modelMatrix, m_currentPos);

    // rotation
    if (m_rotation.has_value()) {
        m_modelMatrix *= anglesToRotationMatrix(m_rotation.value());
    }

    // scaling
    m_modelMatrix = glm::scale(m_modelMatrix, m_size);

    // update normal
    m_normalMatrix = glm::mat3(glm::transpose(glm::inverse(m_modelMatrix)));
}