#include "Weapon.hpp"

bool Weapon::tryShoot(float currentTimeMs)
{
    if (currentTimeMs - m_lastTimeFiredMs >= m_shootDelaysMs[(int)m_type]) {
        m_lastTimeFiredMs = currentTimeMs;
        return true;
    }

    return false;
}
