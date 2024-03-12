#include "Entity.hpp"

#include "glm/geometric.hpp"
#include "GLObject.hpp"
#include "Shader.hpp"

#include <optional>
#include <utility>

BaseEntity::BaseEntity(std::vector<float> vertices,
                       std::optional<std::vector<GLuint>> indices,
                       glm::vec3 pos, glm::vec4 color)
    : shader_("../resources/shaders/shader.vert",
              "../resources/shaders/shader.frag")
    , glObject_(vertices, indices)
    , pos_(pos)
    , color_(color)
{
    this->aabb_[0] = pos - glm::vec3(0.5, 0.5, 0.5);
    this->aabb_[1] = pos + glm::vec3(0.5, 0.5, 0.5);
}

const Shader& BaseEntity::getShader() const
{
    return this->shader_;
}

const glm::vec3& BaseEntity::getPos() const
{
    return this->pos_;
}

const glm::vec4& BaseEntity::getColor() const
{
    return this->color_;
}

IntersectionResult BaseEntity::isIntersectedByLine(const glm::vec3& eyePos,
                                                   const glm::vec3& eyeDir)
{
    IntersectionResult result = std::nullopt;
    float closestDist = FLT_MAX;
    bool intersects = false;

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

            intersects = true;

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

void BaseEntity::move(const glm::vec3& newPos)
{
    this->pos_ = newPos;
    this->aabb_[0] = this->pos_ - glm::vec3(0.5, 0.5, 0.5);
    this->aabb_[1] = this->pos_ + glm::vec3(0.5, 0.5, 0.5);
}

void BaseEntity::render()
{
    this->glObject_.render();
}

bool BaseEntity::isPointInPlaneSection(const glm::vec3& point)
{
    return (point.x <= this->aabb_[1].x && point.x >= this->aabb_[0].x &&
            point.y <= this->aabb_[1].y && point.y >= this->aabb_[0].y &&
            point.z <= this->aabb_[1].z && point.z >= this->aabb_[0].z);
}

void Obstacle::onHit()
{
    // obstacles don't break when hit
}

Target::Target(std::vector<float> vertices,
               std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
               glm::vec4 color)
    : BaseEntity(std::move(vertices), std::move(indices), pos, color)
    , health_(1)
{
}

void Target::onHit()
{
    this->health_--;
    if (this->health_ <= 0)
    {
        this->move({0.0, 0.0, 15.0});
    }
}
