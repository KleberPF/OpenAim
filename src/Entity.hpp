#pragma once

#include "Model.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>

#include <array>
#include <optional>
#include <vector>

using IntersectionResult = std::optional<glm::vec3>;

enum class EntityType {
    TARGET,
    OBSTACLE,
    SPRITE,
};

class CollisionBox
{
public:
    CollisionBox(const glm::vec3& pos);

    /*
    * Returns the closest intersection between this collision box and
    * the line described by eyePos and eyeDir
    */
    IntersectionResult isIntersectedByLine(const glm::vec3& eyePos,
                                           const glm::vec3& eyeDir);

    void move(const glm::vec3& newPos);

private:
    bool isPointInPlaneSection(const glm::vec3& point);

    std::array<glm::vec3, 2> aabb_;
    glm::vec3 pos_;
};

class Entity
{
public:
    Entity(Model model, glm::vec3 pos, glm::vec4 color);
    virtual ~Entity() = default;

    const glm::vec3& getReferentialPos() const;
    const glm::vec3& getCurrentPos() const;
    const glm::vec4& getColor() const;
    EntityType getType() const;

    virtual void onHit()
    {
    }

    void addCollisionBox();

    void move(const glm::vec3& newPos);
    void moveReferential(const glm::vec3& newPos);
    // Moves relative to the referential
    // Position after this should be referentialPos_ + newPos
    void moveRelative(const glm::vec3& newPos);
    void render(const Shader& shader);

    std::optional<CollisionBox>& getCollisionBox();

protected:
    EntityType type_;
    std::optional<CollisionBox> collisionBox_;

    // This holds the actual position the entity is in
    glm::vec3 currentPos_;
    // This holds a reference position used for entity movement.
    // For example, the point where the entity is circling or
    // oscilating around
    glm::vec3 referentialPos_;

private:
    Model model_;
    glm::vec4 color_;
};

class Obstacle : public Entity
{
    Obstacle(Model model, glm::vec3 pos, glm::vec4 color);

    void onHit() override;
};

class Target : public Entity
{
public:
    Target(Model model, glm::vec3 pos, glm::vec4 color);

    void onHit() override;

private:
    int health_;
};
