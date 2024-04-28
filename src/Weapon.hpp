#pragma once

#include "SoundPlayer.hpp"
#include <array>

class Weapon {
public:
    Weapon(SoundPlayer* soundPlayer);

    enum class Type {
        SemiAuto,
        Auto,
        Last,
    };

    bool tryShoot(float currentTimeMs, bool holdingMouseLeft);

    Type type = Type::SemiAuto;

private:
    SoundPlayer* m_soundPlayer = nullptr;

    const std::array<float, (int)Type::Last> m_shootDelaysMs
        = { 100.0f, 25.0f };

    float m_lastTimeFiredMs = 0.0f;
};