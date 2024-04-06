#pragma once

#include "Model.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>

#include <array>
#include <climits>
#include <memory>
#include <optional>
#include <vector>

using IntersectionResult = std::optional<glm::vec3>;

enum class CollisionObjectType {
    AABB,
    SPHERE,
};

class CollisionObject
{
public:
    CollisionObject(const glm::vec3& pos);
    virtual ~CollisionObject() = default;

    virtual void move(const glm::vec3& newPos) = 0;

    /*
    * Returns the closest intersection between this collision object
    * and the line described by eyePos and eyeDir
    */
    virtual IntersectionResult isIntersectedByLine(
        const glm::vec3& eyePos, const glm::vec3& eyeDir) const = 0;

protected:
    glm::vec3 pos_;
};

class CollisionBox : public CollisionObject
{
public:
    CollisionBox(const glm::vec3& pos, const glm::vec3& size);

    void move(const glm::vec3& newPos) override;

    IntersectionResult isIntersectedByLine(
        const glm::vec3& eyePos, const glm::vec3& eyeDir) const override;

private:
    bool isPointInPlaneSection(const glm::vec3& point) const;

    std::array<glm::vec3, 2> aabb_;
    glm::vec3 size_;
};

class CollisionSphere : public CollisionObject
{
public:
    CollisionSphere(const glm::vec3& pos, float radius);

    void move(const glm::vec3& newPos) override;

    IntersectionResult isIntersectedByLine(
        const glm::vec3& eyePos, const glm::vec3& eyeDir) const override;

private:
    float radius_;
};

class Entity
{
public:
    Entity(Model model, const glm::vec3& pos, const glm::vec3& size,
           const glm::vec4& color);
    virtual ~Entity() = default;

    Entity(const Entity& entity) = delete;
    Entity& operator=(const Entity& entity) = delete;

    Entity(Entity&& entity) = default;
    Entity& operator=(Entity&& entity) = default;

    bool destroyable = false;
    int health = INT_MAX;

    const glm::vec3& getReferentialPos() const;
    const glm::vec3& getCurrentPos() const;
    const glm::vec4& getColor() const;
    const glm::vec3& getSize() const;
    const Shader& getShader() const;

    virtual void onHit()
    {
    }

    void move(const glm::vec3& newPos);
    void moveReferential(const glm::vec3& newPos);
    // Moves relative to the referential
    // Position after this should be referentialPos_ + newPos
    void moveRelative(const glm::vec3& newPos);
    void render(const Shader& shader);

    const CollisionObject* getCollisionObject() const;

    void addCollisionObject(CollisionObjectType type);

protected:
    std::unique_ptr<CollisionObject> collisionObject_ = nullptr;

    // This holds the actual position the entity is in
    glm::vec3 currentPos_;
    // This holds a reference position used for entity movement.
    // For example, the point where the entity is circling or
    // oscilating around
    glm::vec3 referentialPos_;

private:
    Model model_;
    glm::vec3 size_;
    glm::vec4 color_;
};
