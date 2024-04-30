#include "Weapon.hpp"

#include "Globals.hpp"

bool Weapon::tryShoot(float currentTimeMs, bool holdingMouseLeft)
{
    if (type == Type::SemiAuto && holdingMouseLeft) {
        return false;
    }

    if (currentTimeMs - m_lastTimeFiredMs >= m_shootDelaysMs[(int)type]) {
        if (type == Type::SemiAuto) {
            g_soundPlayer->playWithRandomPitch("pistol");
        }
        m_lastTimeFiredMs = currentTimeMs;
        return true;
    }

    return false;
}
