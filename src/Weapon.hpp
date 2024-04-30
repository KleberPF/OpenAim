#pragma once

#include <array>

class Weapon {
public:
    Weapon() = default;

    enum class Type {
        SemiAuto,
        Auto,
        Last,
    };

    bool tryShoot(float currentTimeMs, bool holdingMouseLeft);

    Type type = Type::SemiAuto;

private:
    const std::array<float, (int)Type::Last> m_shootDelaysMs
        = { 100.0f, 25.0f };

    float m_lastTimeFiredMs = 0.0f;
};