#pragma once

#include <array>
class Weapon {
public:
    enum class Type {
        Manual,
        Auto,
        Last,
    };

    bool tryShoot(float currentTimeMs);

private:
    const std::array<float, (int)Type::Last> m_shootDelaysMs
        = { 1000.0f, 100.0f };

    Type m_type = Type::Manual;
    float m_lastTimeFiredMs = 0.0f;
};