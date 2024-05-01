#include "EntityManager.hpp"

#include "Entity.hpp"
#include "Globals.hpp"

void EntityManager::addEntity(Entity entity)
{
    m_entities.push_back(std::move(entity));
}

bool EntityManager::updateShotEntities(
    const glm::vec3& eyePos, const glm::vec3& eyeDir)
{
    auto closestEntityIt = m_entities.end();
    float closestDist = FLT_MAX;

    for (auto it = m_entities.begin(); it != m_entities.end(); it++) {
        const auto& entity = *it;
        auto intersection
            = entity.collisionObject()->isIntersectedByLine(eyePos, eyeDir);

        if (intersection.has_value()) {
            if (intersection->dist < closestDist) {
                closestDist = intersection->dist;
                closestEntityIt = it;
            }
        }
    }

    if (closestEntityIt != m_entities.end() && closestEntityIt->destroyable) {
        closestEntityIt->setDamagedThisFrame();
        return true;
    }

    return false;
}

void EntityManager::updateEntities(float timeElapsedSeconds)
{
    auto it = m_entities.begin();
    while (it != m_entities.end()) {
        bool died = it->update(timeElapsedSeconds);
        if (!died) {
            ++it;
            continue;
        }

        if (it->type == Entity::Type::GONER) {
            it = m_entities.erase(it);
        } else if (it->type == Entity::Type::MOVER) {
            float newX = g_rng->getFloatInRange(-8.0f, 8.0f);
            float newY = g_rng->getFloatInRange(2.0f, 18.0f);
            float newZ = -8.0f;
            it->referentialPos = glm::vec3(newX, newY, newZ);
            it->moveRelative(glm::vec3(0.0f, 0.0f,
                0.0f)); // update position after changing referential
            ++it;
        }
    }
}

const std::vector<Entity>& EntityManager::entities() const
{
    return m_entities;
}

CollisionResult EntityManager::areEntitiesColliding(
    const Entity& e1, const Entity& e2)
{
    const CollisionObject* c1 = e1.collisionObject();
    const CollisionObject* c2 = e2.collisionObject();

    if ((c1->type() == CollisionObject::Type::SPHERE
            && c2->type() == CollisionObject::Type::SPHERE)) {
        // sphere on sphere collision
        const auto* s1 = dynamic_cast<const CollisionSphere*>(c1);
        const auto* s2 = dynamic_cast<const CollisionSphere*>(c2);
        float distanceCenters = glm::distance(s1->pos(), s2->pos());

        float penetration = -(distanceCenters - s1->radius() - s2->radius());
        if (penetration > 0) {
            // collision happened
            CollisionData result;
            result.penetration = penetration;
            result.normal = s2->pos() - s1->pos();
            return result;
        }
    }

    return std::nullopt;
}
