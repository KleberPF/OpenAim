#pragma once

#include <irrKlang/irrKlang.h>

#include <array>
class Weapon {
public:
    Weapon(irrklang::ISoundEngine* soundEngine);

    enum class Type {
        SemiAuto,
        Auto,
        Last,
    };

    bool tryShoot(float currentTimeMs, bool holdingMouseLeft);

private:
    const std::array<float, (int)Type::Last> m_shootDelaysMs
        = { 100.0f, 25.0f };

    Type m_type = Type::SemiAuto;
    float m_lastTimeFiredMs = 0.0f;
    irrklang::ISoundEngine* m_soundEngine;
};