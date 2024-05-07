#pragma once

#include "Entity.hpp"
#include "Weapon.hpp"

#include <glm/glm.hpp>

struct Target {
    enum class Shape {
        Box,
        Ball,
    };

    glm::vec3 scale = glm::vec3(1.0f);
    bool randomSpawn = true;
    glm::vec3 spawnCoords;
    glm::vec3 minCoords;
    glm::vec3 maxCoords;
    Entity::Type type;
    bool moves = false;
    glm::vec3 movementDirection = glm::vec3(1.0f, 0.0f, 0.0f);
    float movementAmplitude;
    float movementSpeed;
    int health = 1;
    Shape shape;
};

struct Scenario {
    std::string name;
    Weapon::Type weaponType = Weapon::Type::Pistol;
    glm::vec3 playerPos;
    std::vector<Target> targets;
};
