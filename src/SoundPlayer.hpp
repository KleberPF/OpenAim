#pragma once

#include "RNG.hpp"
#include "ResourceManager.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <map>
#include <string>

class SoundPlayer {
public:
    SoundPlayer();
    ~SoundPlayer();

    void play(const std::string& soundName);
    void playWithRandomPitch(const std::string& soundName);
    void playIfNotAlreadyPlaying(const std::string& soundName);

private:
    std::map<std::string, ALuint> m_sources;

    // OpenAL stuff
    ALCdevice* m_device;
    ALCcontext* m_context;
    bool m_eax;
};