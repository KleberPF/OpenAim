#pragma once

#include "Model.hpp"
#include "Renderer.hpp"
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

struct Rotation {
    Rotation()
        : axis(glm::vec3(0.0f, 1.0f, 0.0f))
    {
    }

    Rotation(float _angle, const glm::vec3& _axis)
        : angle(_angle)
        , axis(_axis)
    {
    }

    float angle = 0.0f;
    glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);
};

class CollisionObject {
public:
    CollisionObject(const glm::vec3& pos);
    virtual ~CollisionObject() = default;

    virtual void move(const glm::vec3& newPos) = 0;

    /*
     * Returns the closest intersection between this collision object
     * and the line described by eyePos and eyeDir
     */
    virtual IntersectionResult isIntersectedByLine(glm::vec3 eyePos,
        glm::vec3 eyeDir) const
        = 0;

    void setRotation(const Rotation& rotation);
    const Rotation& getRotation() const;

protected:
    glm::vec3 m_pos;

    // As the name suggests, AABBs are axis aligned. Because the collision
    // object is invisible anyways, we aren't actually going to rotate it.
    // Instead, we rotate the eyePos and eyeDir when calculating the
    // intersection.
    Rotation m_rotation;
};

class CollisionBox : public CollisionObject {
public:
    CollisionBox(const glm::vec3& pos, const glm::vec3& size);

    void move(const glm::vec3& newPos) override;

    IntersectionResult isIntersectedByLine(glm::vec3 eyePos,
        glm::vec3 eyeDir) const override;

private:
    bool isPointInPlaneSection(const glm::vec3& point) const;

    std::array<glm::vec3, 2> m_aabb;
    glm::vec3 m_size;
};

class CollisionSphere : public CollisionObject {
public:
    CollisionSphere(const glm::vec3& pos, float radius);

    void move(const glm::vec3& newPos) override;

    IntersectionResult isIntersectedByLine(glm::vec3 eyePos,
        glm::vec3 eyeDir) const override;

private:
    float m_radius;
};

class Entity {
public:
    Entity(Model model, const glm::vec3& pos);
    virtual ~Entity() = default;

    Entity(const Entity& entity) = delete;
    Entity& operator=(const Entity& entity) = delete;

    Entity(Entity&& entity) = default;
    Entity& operator=(Entity&& entity) = default;

    const Rotation& getRotation() const;
    const glm::vec3& getReferentialPos() const;
    const glm::vec3& getCurrentPos() const;
    const Shader& getShader() const;

    virtual void onHit()
    {
    }

    void rotate(float angle, const glm::vec3& axis);
    void move(const glm::vec3& newPos);
    void moveReferential(const glm::vec3& newPos);
    // Moves relative to the referential
    // Position after this should be referentialPos_ + newPos
    void moveRelative(const glm::vec3& newPos);

    const CollisionObject* getCollisionObject() const;

    void addCollisionObject(CollisionObjectType type);

    bool isDestroyable() const;
    void setDestroyable(bool destroyable);

    int getHealth() const;
    void setHealth(int health);

    void setSize(const glm::vec3& size);
    void setColor(const glm::vec3& color);
    void setRotation(const Rotation& rotation);

    friend class Renderer;

private:
    std::unique_ptr<CollisionObject> m_collisionObject = nullptr;

    bool m_destroyable = false;
    int m_health = 1;
    glm::vec3 m_size = glm::vec3(1.0f);
    glm::vec3 m_color = glm::vec3(0.0f);
    Rotation m_rotation;

    // This holds the actual position the entity is in
    glm::vec3 m_currentPos;
    // This holds a reference position used for entity movement.
    // For example, the point where the entity is circling or
    // oscilating around
    glm::vec3 m_referentialPos;

    Model m_model;
};
