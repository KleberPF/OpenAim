#pragma once

#include "Entity.hpp"
#include "Weapon.hpp"

#include <glm/glm.hpp>

struct Target {
    enum class Shape {
        Box,
        Ball,
    };

    Entity::Type type = Entity::Type::GONER;
    Shape shape = Shape::Ball;
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 spawnCoords;
    glm::vec3 minCoords;
    glm::vec3 maxCoords;
    bool randomSpawn = true;
    bool moves = false;
    float movementAmplitude;
    float movementSpeed;
    int health = 1;
};

struct Scenario {
    std::string name;
    Weapon::Type weaponType = Weapon::Type::Pistol;
    glm::vec3 playerPos;
    std::vector<Target> targets;
};
