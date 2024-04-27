#pragma once

#include "Model.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <optional>

struct Intersection {
    Intersection(const glm::vec3& coord, float dist)
        : coord(coord)
        , dist(dist)
    {
    }

    glm::vec3 coord;
    float dist;
};

using IntersectionResult = std::optional<Intersection>;

enum class CollisionObjectType {
    AABB,
    SPHERE,
};

// Each coordinate represents the rotation along
// the main axis
using Rotation = glm::vec3;

class CollisionObject {
public:
    CollisionObject(const glm::vec3& pos);
    virtual ~CollisionObject() = default;

    virtual void move(const glm::vec3& newPos) = 0;

    /*
     * Returns the closest intersection between this collision object
     * and the line described by eyePos and eyeDir
     */
    virtual IntersectionResult isIntersectedByLine(
        glm::vec3 eyePos, glm::vec3 eyeDir) const
        = 0;

    void setRotation(const Rotation& rotation);
    std::optional<Rotation> getRotation() const;

    virtual void setSize(const glm::vec3& size) = 0;

protected:
    glm::vec3 m_pos;

    // As the name suggests, AABBs are axis aligned. Because the collision
    // object is invisible anyways, we aren't actually going to rotate it.
    // Instead, we rotate the eyePos and eyeDir when calculating the
    // intersection.
    std::optional<Rotation> m_rotation;
};

class CollisionBox : public CollisionObject {
public:
    CollisionBox(const glm::vec3& pos, const glm::vec3& size);

    void move(const glm::vec3& newPos) override;

    IntersectionResult isIntersectedByLine(
        glm::vec3 eyePos, glm::vec3 eyeDir) const override;

    void setSize(const glm::vec3& size) override;

private:
    bool isPointInPlaneSection(const glm::vec3& point) const;

    // TODO: we probably don't need aabb and size
    // remove one of them in the future
    std::array<glm::vec3, 2> m_aabb;
    glm::vec3 m_size;
};

class CollisionSphere : public CollisionObject {
public:
    CollisionSphere(const glm::vec3& pos, float radius);

    void move(const glm::vec3& newPos) override;

    IntersectionResult isIntersectedByLine(
        glm::vec3 eyePos, glm::vec3 eyeDir) const override;

    void setSize(const glm::vec3& size) override;

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

    glm::mat4 getModelMatrix() const;
    glm::mat3 getNormalMatrix() const;

    std::optional<Rotation> getRotation() const;
    const glm::vec3& getReferentialPos() const;
    const glm::vec3& getCurrentPos() const;
    const Shader& getShader() const;
    const Material& getMaterial() const;

    void setRotation(float x, float y, float z);
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

    const std::string& getName() const;
    void setName(const std::string& name);

    void setMovementPattern(std::function<glm::vec3(float)> callback);
    void update(float timePassedSeconds);

    void render() const;

    // This holds a reference position used for entity movement.
    // For example, the point where the entity is circling or
    // oscilating around
    glm::vec3 referentialPos;

private:
    // both are meant to be called after a move, resize or rotation
    void updateMatrices();

    glm::mat4 m_modelMatrix;
    glm::mat3 m_normalMatrix;

    std::unique_ptr<CollisionObject> m_collisionObject = nullptr;

    // This function is used to determine the new position of this entity
    // given the current time of the application
    // If null, we are going to assume this entity isn't moving
    std::function<glm::vec3(float)> m_calculateNewPos = nullptr;

    std::string m_name;
    bool m_destroyable = false;
    int m_health = 1;
    glm::vec3 m_size = glm::vec3(1.0f);
    glm::vec3 m_color = glm::vec3(0.0f);
    std::optional<Rotation> m_rotation;

    // This holds the actual position the entity is in
    glm::vec3 m_currentPos;

    Model m_model;
};
