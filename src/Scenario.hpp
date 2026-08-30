#pragma once

#include "Weapon.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <string>

struct Target;
struct Scenario {
    enum class WinCondition : uint8_t {
        ClearTargets,
        Time,
    };

    struct Coordinate {
        double x;
        double y;
        double z;
    };

    std::string name;
    Weapon::Type weaponType = Weapon::Type::Pistol;
    Coordinate playerPos;
    WinCondition winCondition = WinCondition::Time;
    float challengeDurationSeconds;
    std::vector<Target> targets;
};

// (referentialPos, currentPos, timeElapsed)
using PositionerCallback = std::function<Scenario::Coordinate(Scenario::Coordinate,Scenario::Coordinate,double)>;

struct Target {
    enum class Shape : uint8_t {
        Box,
        Ball,
    };

    enum class Type : uint8_t {
        Mover, // when dead, move to a random place and regenerate health
        Goner, // when dead, die for good
    };

    Type type = Type::Goner;
    Shape shape = Shape::Ball;
    glm::vec3 scale = glm::vec3(1.0f);
    Scenario::Coordinate spawnCoords;
    Scenario::Coordinate minCoords;
    Scenario::Coordinate maxCoords;
    bool randomSpawn = true;
    int health = 1;

    // Calculates position per frame for moving target
    // Takes in the current time, spits out a coordinate
    PositionerCallback positioner = nullptr;
};
