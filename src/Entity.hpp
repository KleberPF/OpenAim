#pragma once

#include "Material.hpp"
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

// Each coordinate represents the rotation along
// the main axis
using Rotation = glm::vec3;

class CollisionObject {
public:
    enum class Type {
        AABB,
        SPHERE,
    };

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

    virtual void setSize(const glm::vec3& size) = 0;

    Type type() const;
    const glm::vec3& pos() const;

protected:
    Type m_type;
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

    float radius() const;

private:
    float m_radius;
};

class Entity {
public:
    enum class Type {
        MOVER, // when dead, move to a random place and regenerate health
        GONER, // when dead, die for good
    };

    Entity(Model model, const Material& material, const Shader& shader,
        const glm::vec3& pos);
    virtual ~Entity() = default;

    Entity(const Entity& entity) = delete;
    Entity& operator=(const Entity& entity) = delete;

    Entity(Entity&& entity) = default;
    Entity& operator=(Entity&& entity) = default;

    glm::mat4 modelMatrix() const;
    glm::mat3 normalMatrix() const;
    glm::mat4 buildHealthbarModelMatrix() const;

    float getHealthPercentage() const;
    glm::vec3 getHealthBarColor() const;

    void setRotation(float x, float y, float z);
    void move(const glm::vec3& newPos);
    // Moves relative to the referential
    // Position after this should be referentialPos + newPos
    void moveRelative(const glm::vec3& newPos);

    void addCollisionObject(CollisionObject::Type type);
    const CollisionObject* collisionObject() const;

    bool isDestroyable() const;
    void setDestroyable(bool destroyable);

    void setSize(const glm::vec3& size);

    const std::string& getName() const;
    void setName(const std::string& name);

    void setStartingHealth(int health);
    void setDamagedThisFrame();

    void setMovementPattern(std::function<glm::vec3(float)> callback);

    // returns whether entity should die
    bool update(float timePassedSeconds);

    bool shouldRenderHealthBar() const;

    void render() const;
    void renderHealthBar() const;

    // This holds a reference position used for entity movement.
    // For example, the point where the entity is circling or
    // oscilating around
    glm::vec3 referentialPos;
    bool destroyable = false;
    Type type = Type::GONER;

    std::reference_wrapper<const Material> material2;
    std::reference_wrapper<const Shader> shader2;

    std::reference_wrapper<Material> healthbarMaterial;
    std::reference_wrapper<const Shader> healthbarShader;

private:
    // both are meant to be called after a move, resize or rotation
    void updateMatrices();

    glm::mat4 m_modelMatrix;
    glm::mat3 m_normalMatrix;
    glm::mat4 m_healthbarModelMatrix;

    std::unique_ptr<CollisionObject> m_collisionObject = nullptr;

    // This function is used to determine the new position of this entity
    // given the current time of the application
    // If null, we are going to assume this entity isn't moving
    std::function<glm::vec3(float)> m_calculateNewPos = nullptr;

    std::string m_name;
    int m_startingHealth = 1;
    int m_currentHealth = 1;
    bool m_damagedThisFrame = false;

    /*
     * This is basically the scale of the entity
     * The default sizes of the models are:
     * Cube: 1x1x1
     * Plane: 1x1
     * Sphere: Diameter 1
     */
    glm::vec3 m_size = glm::vec3(1.0f);
    std::optional<Rotation> m_rotation;

    // This holds the actual position the entity is in
    glm::vec3 m_currentPos;

    Model m_model;

    Model m_healthbarQuad;
};
