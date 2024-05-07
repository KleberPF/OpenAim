#pragma once

#include "Entity.hpp"
#include "Weapon.hpp"

#include <glm/glm.hpp>

struct Target {
    glm::vec3 scale = glm::vec3(1.0f);
    bool randomSpawn = true;
    glm::vec3 spawnCoords;
    glm::vec3 minCoords;
    glm::vec3 maxCoords;
    Entity::Type type;
};

struct Scenario {
    std::string name;
    Weapon::Type weaponType = Weapon::Type::Pistol;
    glm::vec3 playerPos;
    std::vector<Target> targets;
};
