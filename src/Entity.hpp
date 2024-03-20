#pragma once

#include "GLObject.hpp"
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
    Entity(std::vector<float> vertices,
           std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
           glm::vec4 color);

    Entity(const Entity& entity) = default;
    Entity& operator=(const Entity& entity) = default;

    Entity(Entity&& entity) = default;
    Entity& operator=(Entity&& entity) = default;
    virtual ~Entity() = default;

    const Shader& getShader() const;
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
    void render();

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
    Shader shader_;
    GLObject glObject_;
    glm::vec4 color_;
};

class Obstacle : public Entity
{
    Obstacle(std::vector<float> vertices,
             std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
             glm::vec4 color);

    void onHit() override;
};

class Target : public Entity
{
public:
    Target(std::vector<float> vertices,
           std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
           glm::vec4 color);

    void onHit() override;

private:
    int health_;
};

class Sprite : public Entity
{
public:
    Sprite(std::vector<float> vertices,
           std::optional<std::vector<GLuint>> indices, glm::vec3 pos,
           glm::vec4 color);
};