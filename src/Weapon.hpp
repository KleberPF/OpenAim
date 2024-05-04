#pragma once

#include <array>

class Weapon {
public:
    Weapon() = default;

    enum class Type {
        Pistol,
        Machine_Gun,
        Last,
    };

    bool tryShoot(float currentTimeMs, bool holdingMouseLeft);

    Type type = Type::Pistol;

private:
    const std::array<float, (int)Type::Last> m_shootDelaysMs
        = { 100.0f, 25.0f };

    float m_lastTimeFiredMs = 0.0f;
};