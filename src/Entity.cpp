#include "Entity.hpp"

#include "Model.hpp"
#include "Shader.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include "utils.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>

CollisionObject::CollisionObject(const glm::vec3& pos)
    : m_pos(pos)
{
}

void CollisionObject::setRotation(const Rotation& rotation)
{
    m_rotation = rotation;
}

std::optional<Rotation> CollisionObject::getRotation() const
{
    return m_rotation;
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
        glm::mat4 rotateEye
            = glm::inverse(anglesToRotationMatrix(m_rotation.value()));
        glm::mat4 moveBack = glm::translate(glm::identity<glm::mat4>(), m_pos);

        auto tmp = glm::vec4(eyePos, 1.0f);
        eyePos = glm::vec3(moveBack * rotateEye * moveToOrigin * tmp);

        tmp = glm::vec4(eyeDir, 1.0f);
        eyeDir = glm::vec3(rotateEye * tmp);
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
                result = intersection;
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
        return eyePos + eyeDir * d;
    }

    return std::nullopt;
}

void CollisionSphere::setSize(const glm::vec3& size)
{
    m_radius = size.x;
}

Entity::Entity(Model model, const glm::vec3& pos)
    : m_currentPos(pos)
    , m_referentialPos(pos)
    , m_model(std::move(model))
{
}

std::optional<Rotation> Entity::getRotation() const
{
    return m_rotation;
}

const glm::vec3& Entity::getReferentialPos() const
{
    return m_referentialPos;
}

const glm::vec3& Entity::getCurrentPos() const
{
    return m_currentPos;
}

const Shader& Entity::getShader() const
{
    return m_model.getShader();
}

void Entity::setRotation(float x, float y, float z)
{
    m_rotation = glm::vec3(x, y, z);
    if (m_collisionObject != nullptr) {
        m_collisionObject->setRotation(m_rotation.value());
    }
}

void Entity::move(const glm::vec3& newPos)
{
    m_currentPos = newPos;
    if (m_collisionObject != nullptr) {
        m_collisionObject->move(newPos);
    }
}

void Entity::moveReferential(const glm::vec3& newPos)
{
    m_referentialPos = newPos;
    move(newPos);
}

void Entity::moveRelative(const glm::vec3& newPos)
{
    move(m_referentialPos + newPos);
}

const CollisionObject* Entity::getCollisionObject() const
{
    return m_collisionObject.get();
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

bool Entity::isDestroyable() const
{
    return m_destroyable;
}

void Entity::setDestroyable(bool destroyable)
{
    m_destroyable = destroyable;
}

int Entity::getHealth() const
{
    return m_health;
}

void Entity::setHealth(int health)
{
    m_health = health;
}

void Entity::setSize(const glm::vec3& size)
{
    m_size = size;
    if (m_collisionObject != nullptr) {
        m_collisionObject->setSize(size);
    }
}

void Entity::setColor(const glm::vec3& color)
{
    m_color = color;
}
