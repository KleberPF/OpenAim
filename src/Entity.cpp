#include "Entity.hpp"

#include "glm/ext/matrix_transform.hpp"
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

IntersectionResult CollisionBox::isIntersectedByLine(glm::vec3 eyePos,
                                                     glm::vec3 eyeDir) const
{
    IntersectionResult result = std::nullopt;
    float closestDist = FLT_MAX;

    if (this->rotation.angle != 0.0f)
    {
        glm::mat4 moveToOrigin =
            glm::translate(glm::identity<glm::mat4>(), -this->pos_);
        glm::mat4 rotateEye = glm::rotate(glm::identity<glm::mat4>(),
                                          glm::radians(-this->rotation.angle),
                                          this->rotation.axis);
        glm::mat4 moveBack =
            glm::translate(glm::identity<glm::mat4>(), this->pos_);

        auto tmp = glm::vec4(eyePos, 1.0f);
        eyePos = glm::vec3(moveBack * rotateEye * moveToOrigin * tmp);

        tmp = glm::vec4(eyeDir, 1.0f);
        eyeDir = glm::vec3(rotateEye * tmp);
    }

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
IntersectionResult CollisionSphere::isIntersectedByLine(glm::vec3 eyePos,
                                                        glm::vec3 eyeDir) const
{
    float delta =
        pow(glm::dot(eyeDir, eyePos - this->pos_), 2) -
        (pow(glm::length(eyePos - this->pos_), 2) - pow(this->radius_, 2));

    if (delta < 0)
    {
        return std::nullopt;
    }

    float d = -(glm::dot(eyeDir, eyePos - this->pos_)) - std::sqrt(delta);

    if (d > 0)
    {
        return eyePos + eyeDir * d;
    }

    return std::nullopt;
}

Entity::Entity(Model model, const glm::vec3& pos)
    : currentPos_(pos)
    , referentialPos_(pos)
    , model_(std::move(model))
{
}

const Rotation& Entity::getRotation() const
{
    return this->rotation_;
}

const glm::vec3& Entity::getReferentialPos() const
{
    return this->referentialPos_;
}

const glm::vec3& Entity::getCurrentPos() const
{
    return this->currentPos_;
}

const Shader& Entity::getShader() const
{
    return this->model_.getShader();
}

void Entity::rotate(float angle, const glm::vec3& axis)
{
    this->rotation_ = {angle, axis};
    if (this->collisionObject_ != nullptr)
    {
        this->collisionObject_->rotation = {angle, axis};
    }
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

const CollisionObject* Entity::getCollisionObject() const
{
    return this->collisionObject_.get();
}

void Entity::addCollisionObject(CollisionObjectType type)
{
    switch (type)
    {
        case CollisionObjectType::SPHERE:
            this->collisionObject_ = std::make_unique<CollisionSphere>(
                this->currentPos_, this->size.x);
            break;
        default:
            this->collisionObject_ =
                std::make_unique<CollisionBox>(this->currentPos_, this->size);
            break;
    }

    this->collisionObject_->rotation = this->rotation_;
}
