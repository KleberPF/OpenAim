#include "Weapon.hpp"

#include "Globals.hpp"

bool Weapon::tryShoot(float currentTimeMs, bool holdingMouseLeft)
{
    if (type == Type::Pistol && holdingMouseLeft) {
        return false;
    }

    if (currentTimeMs - m_lastTimeFiredMs >= m_shootDelaysMs[(int)type]) {
        if (type == Type::Pistol) {
            g_soundPlayer->playWithRandomPitch("pistol");
        } else if (type == Type::Machine_Gun) {
            g_soundPlayer->playIfNotAlreadyPlaying("machine_gun");
        }
        m_lastTimeFiredMs = currentTimeMs;
        return true;
    }

    return false;
}
