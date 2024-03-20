#include "Entity.hpp"

#include "GLObject.hpp"
#include "Shader.hpp"

#include <optional>
#include <utility>

CollisionBox::CollisionBox(const glm::vec3& pos)
    : pos_(pos)
{
    this->aabb_[0] = pos - glm::vec3(0.5, 0.5, 0.5);
    this->aabb_[1] = pos + glm::vec3(0.5, 0.5, 0.5);
}

IntersectionResult CollisionBox::isIntersectedByLine(const glm::vec3& eyePos,
                                                     const glm::vec3& eyeDir)
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

bool CollisionBox::isPointInPlaneSection(const glm::vec3& point)
{
    return (point.x <= this->aabb_[1].x && point.x >= this->aabb_[0].x &&
            point.y <= this->aabb_[1].y && point.y >= this->aabb_[0].y &&
            point.z <= this->aabb_[1].z && point.z >= this->aabb_[0].z);
}

void CollisionBox::move(const glm::vec3& newPos)
{
    this->pos_ = newPos;
    this->aabb_[0] = this->pos_ - glm::vec3(0.5, 0.5, 0.5);
    this->aabb_[1] = this->pos_ + glm::vec3(0.5, 0.5, 0.5);
}

Entity::Entity(std::vector<float> vertices,
               std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
               glm::vec4 color)
    : shader_("../resources/shaders/shader.vert",
              "../resources/shaders/shader.frag")
    , glObject_(vertices, indices)
    , referentialPos_(pos)
    , currentPos_(pos)
    , color_(color)
{
}

const Shader& Entity::getShader() const
{
    return this->shader_;
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

EntityType Entity::getType() const
{
    return this->type_;
}

void Entity::addCollisionBox()
{
    if (this->collisionBox_.has_value())
    {
        return;
    }

    this->collisionBox_ = CollisionBox(this->currentPos_);
}

void Entity::move(const glm::vec3& newPos)
{
    this->currentPos_ = newPos;
    if (this->collisionBox_.has_value())
    {
        this->collisionBox_->move(newPos);
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

void Entity::render()
{
    this->glObject_.render();
}

std::optional<CollisionBox>& Entity::getCollisionBox()
{
    return this->collisionBox_;
}

Obstacle::Obstacle(std::vector<float> vertices,
                   std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
                   glm::vec4 color)
    : Entity(std::move(vertices), std::move(indices), pos, color)
{
    this->type_ = EntityType::OBSTACLE;
}

void Obstacle::onHit()
{
    // obstacles don't break when hit
}

Target::Target(std::vector<float> vertices,
               std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
               glm::vec4 color)
    : Entity(std::move(vertices), std::move(indices), pos, color)
    , health_(1)
{
    this->type_ = EntityType::TARGET;
    this->addCollisionBox();
}

void Target::onHit()
{
    this->health_--;
    if (this->health_ <= 0)
    {
        this->moveReferential({0.0, 0.0, 15.0});
    }
}

Sprite::Sprite(std::vector<float> vertices,
               std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
               glm::vec4 color)
    : Entity(std::move(vertices), std::move(indices), pos, color)
{
}
