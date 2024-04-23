#pragma once

#include <random>

class RNG {
public:
    RNG();

    float getFloatInRange(float leftInclusive, float rightInclusive);

private:
    std::mt19937 m_mt;
};