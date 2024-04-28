#include "Weapon.hpp"

Weapon::Weapon(SoundPlayer* soundPlayer)
    : m_soundPlayer(soundPlayer)
{
}

bool Weapon::tryShoot(float currentTimeMs, bool holdingMouseLeft)
{
    if (type == Type::SemiAuto && holdingMouseLeft) {
        return false;
    }

    if (currentTimeMs - m_lastTimeFiredMs >= m_shootDelaysMs[(int)type]) {
        if (type == Type::SemiAuto && m_soundPlayer != nullptr) {
            m_soundPlayer->play("pistol");
        }
        m_lastTimeFiredMs = currentTimeMs;
        return true;
    }

    return false;
}
