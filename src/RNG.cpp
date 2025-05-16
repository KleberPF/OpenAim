#include "RNG.hpp"

#include <cassert>
#include <memory>

namespace {

std::unique_ptr<RNG> s_Instance = nullptr;

} // end namespace

void RNG::init()
{
    if (s_Instance != nullptr) {
        return;
    }

    s_Instance = std::unique_ptr<RNG>(new RNG);
}

void RNG::shutdown()
{
    s_Instance.reset();
}

RNG& RNG::instance()
{
    assert(s_Instance);
    return *s_Instance;
}

RNG::RNG()
    : m_mt(std::random_device {}())
{
}

float RNG::getFloatInRange(float leftInclusive, float rightInclusive)
{
    std::uniform_real_distribution<> dist(leftInclusive, rightInclusive);
    return dist(m_mt);
}