#pragma once

#include "RNG.hpp"
#include "ResourceManager.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <map>
#include <string>

class SoundPlayer {
public:
    SoundPlayer(const ResourceManager& resourceManager, RNG& rng);
    ~SoundPlayer();

    void play(const std::string& soundName);
    void playWithRandomPitch(const std::string& soundName);

private:
    const ResourceManager& m_resourceManager;
    RNG& m_rng;
    std::map<std::string, ALuint> m_sources;

    // OpenAL stuff
    ALCdevice* m_device;
    ALCcontext* m_context;
    bool m_eax;
};