#pragma once

#include <irrKlang/irrKlang.h>

#include <array>
class Weapon {
public:
    Weapon(irrklang::ISoundEngine* soundEngine);

    enum class Type {
        Manual,
        Auto,
        Last,
    };

    bool tryShoot(float currentTimeMs);

private:
    const std::array<float, (int)Type::Last> m_shootDelaysMs
        = { 100.0f, 25.0f };

    Type m_type = Type::Manual;
    float m_lastTimeFiredMs = 0.0f;
    irrklang::ISoundEngine* m_soundEngine;
};