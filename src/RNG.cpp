#include "RNG.hpp"

RNG::RNG()
    : m_mt(std::random_device {}())
{
}

float RNG::getFloatInRange(float leftInclusive, float rightInclusive)
{
    std::uniform_real_distribution<> dist(leftInclusive, rightInclusive);
    return dist(m_mt);
}