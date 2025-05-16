#include "Weapon.hpp"

#include "SoundPlayer.hpp"

bool Weapon::tryShoot(float currentTimeMs, bool holdingMouseLeft)
{
    if (type == Type::Pistol && holdingMouseLeft) {
        return false;
    }

    if (currentTimeMs - m_lastTimeFiredMs >= m_shootDelaysMs[(int)type]) {
        if (type == Type::Pistol) {
            SoundPlayer::instance().playWithRandomPitch("pistol");
        } else if (type == Type::Machine_Gun) {
            SoundPlayer::instance().playIfNotAlreadyPlaying("machine_gun");
        }
        m_lastTimeFiredMs = currentTimeMs;
        return true;
    }

    return false;
}
