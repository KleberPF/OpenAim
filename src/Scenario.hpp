#pragma once

#include "Entity.hpp"
#include "Weapon.hpp"

#include <glm/glm.hpp>

struct Target {
    float scale;
    bool randomSpawn;
    glm::vec3 spawnCoords;
    glm::vec3 minCoords;
    glm::vec3 maxCoords;
    Entity::Type type;
};

struct Scenario {
    std::string name;
    Weapon::Type weaponType;
    glm::vec3 playerPos;
    bool canPlayerMove;
    std::vector<Target> targets;
};
