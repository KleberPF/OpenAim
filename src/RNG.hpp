#pragma once

#include <random>

class RNG {
public:
    static void init();
    static void shutdown();

    static RNG& instance(); // maybe const this?

    float getFloatInRange(float leftInclusive, float rightInclusive);

private:
    RNG();

    std::mt19937 m_mt;
};