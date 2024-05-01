#pragma once

#include "Entity.hpp"

struct CollisionData {
    float penetration;
    glm::vec3 normal;
};

using CollisionResult = std::optional<CollisionData>;

class EntityManager {
public:
    EntityManager() = default;

    void addEntity(Entity entity);
    // returns whether an entity was hit
    bool updateShotEntities(const glm::vec3& eyePos, const glm::vec3& eyeDir);
    void updateEntities(float timeElapsedSeconds);

    const std::vector<Entity>& entities() const;

private:
    static CollisionResult areEntitiesColliding(
        const Entity& e1, const Entity& e2);

    std::vector<Entity> m_entities;
};