#include "Weapon.hpp"

Weapon::Weapon(irrklang::ISoundEngine* soundEngine)
    : m_soundEngine(soundEngine)
{
}

bool Weapon::tryShoot(float currentTimeMs, bool holdingMouseLeft)
{
    if (type == Type::SemiAuto && holdingMouseLeft) {
        return false;
    }

    if (currentTimeMs - m_lastTimeFiredMs >= m_shootDelaysMs[(int)type]) {
        m_lastTimeFiredMs = currentTimeMs;
        m_soundEngine->play2D("../resources/sounds/pistol.wav");
        return true;
    }

    return false;
}
