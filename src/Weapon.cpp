#include "Weapon.hpp"

bool Weapon::tryShoot(float currentTimeMs, bool holdingMouseLeft)
{
    if (type == Type::SemiAuto && holdingMouseLeft) {
        return false;
    }

    if (currentTimeMs - m_lastTimeFiredMs >= m_shootDelaysMs[(int)type]) {
        m_lastTimeFiredMs = currentTimeMs;
        return true;
    }

    return false;
}
