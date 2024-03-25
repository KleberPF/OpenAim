#include "Entity.hpp"

#include "glm/geometric.hpp"
#include "Model.hpp"
#include "Shader.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>

CollisionObject::CollisionObject(const glm::vec3& pos)
    : pos_(pos)
{
}

CollisionBox::CollisionBox(const glm::vec3& pos, const glm::vec3& size)
    : CollisionObject(pos)
    , size_(size)
{
    this->aabb_[0] = this->pos_ - glm::vec3(0.5, 0.5, 0.5) * this->size_;
    this->aabb_[1] = this->pos_ + glm::vec3(0.5, 0.5, 0.5) * this->size_;
}

IntersectionResult CollisionBox::isIntersectedByLine(
    const glm::vec3& eyePos, const glm::vec3& eyeDir) const
{
    IntersectionResult result = std::nullopt;
    float closestDist = FLT_MAX;

    for (const auto& vertex : this->aabb_)
    {
        for (size_t j = 0; j < 3; j++)
        {
            float t = (vertex[j] - eyePos[j]) / eyeDir[j];
            if (t < 0)
            {
                continue;
            }

            glm::vec3 intersection = eyePos + eyeDir * t;
            bool intersectsNow = this->isPointInPlaneSection(intersection);

            if (!intersectsNow)
            {
                continue;
            }

            float dist = glm::distance(intersection, eyePos);
            if (dist < closestDist)
            {
                closestDist = dist;
                result = intersection;
            }
        }
    }

    return result;
}

void CollisionBox::move(const glm::vec3& newPos)
{
    this->pos_ = newPos;
    this->aabb_[0] = this->pos_ - glm::vec3(0.5, 0.5, 0.5) * this->size_;
    this->aabb_[1] = this->pos_ + glm::vec3(0.5, 0.5, 0.5) * this->size_;
}

bool CollisionBox::isPointInPlaneSection(const glm::vec3& point) const
{
    return (point.x <= this->aabb_[1].x && point.x >= this->aabb_[0].x &&
            point.y <= this->aabb_[1].y && point.y >= this->aabb_[0].y &&
            point.z <= this->aabb_[1].z && point.z >= this->aabb_[0].z);
}

CollisionSphere::CollisionSphere(const glm::vec3& pos, float radius)
    : CollisionObject(pos)
    , radius_(radius)
{
}

void CollisionSphere::move(const glm::vec3& newPos)
{
    this->pos_ = newPos;
}

// https://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
IntersectionResult CollisionSphere::isIntersectedByLine(
    const glm::vec3& eyePos, const glm::vec3& eyeDir) const
{
    glm::vec3 eyeDirNorm = glm::normalize(eyeDir);
    float delta =
        pow(glm::dot(eyeDirNorm, eyePos - this->pos_), 2) -
        (pow(glm::length(eyePos - this->pos_), 2) - pow(this->radius_, 2));

    if (delta < 0)
    {
        return std::nullopt;
    }

    float d = -(glm::dot(eyeDirNorm, eyePos - this->pos_)) - std::sqrt(delta);

    if (d > 0)
    {
        return eyePos + eyeDir * d;
    }

    return std::nullopt;
}

Entity::Entity(Model model, const glm::vec3& pos, const glm::vec3& size,
               const glm::vec4& color)
    : currentPos_(pos)
    , referentialPos_(pos)
    , model_(std::move(model))
    , size_(size)
    , color_(color)
{
}

const glm::vec3& Entity::getReferentialPos() const
{
    return this->referentialPos_;
}

const glm::vec3& Entity::getCurrentPos() const
{
    return this->currentPos_;
}

const glm::vec4& Entity::getColor() const
{
    return this->color_;
}

const glm::vec3& Entity::getSize() const
{
    return this->size_;
}

void Entity::move(const glm::vec3& newPos)
{
    this->currentPos_ = newPos;
    if (this->collisionObject_ != nullptr)
    {
        this->collisionObject_->move(newPos);
    }
}

void Entity::moveReferential(const glm::vec3& newPos)
{
    this->referentialPos_ = newPos;
    this->move(newPos);
}

void Entity::moveRelative(const glm::vec3& newPos)
{
    this->move(this->referentialPos_ + newPos);
}

void Entity::render(const Shader& shader)
{
    this->model_.render(shader);
}

const CollisionObject* Entity::getCollisionObject() const
{
    return this->collisionObject_.get();
}

void Entity::addCollisionObject(CollisionObjectType type)
{
    switch (type)
    {
        case CollisionObjectType::AABB:
            this->collisionObject_ =
                std::make_unique<CollisionBox>(this->currentPos_, this->size_);
            break;
        case CollisionObjectType::SPHERE:
            this->collisionObject_ = std::make_unique<CollisionSphere>(
                this->currentPos_, this->size_.x);
            break;
    }
}
