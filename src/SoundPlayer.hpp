#pragma once

#include "ResourceManager.hpp"

#include <AL/al.h>
#include <AL/alc.h>

#include <map>
#include <string>

class SoundPlayer {
public:
    SoundPlayer(const ResourceManager& resourceManager);
    ~SoundPlayer();

    void play(const std::string& soundName);

private:
    const ResourceManager& m_resourceManager;
    std::map<std::string, ALuint> m_sources;

    // OpenAL stuff
    ALCdevice* m_device;
    ALCcontext* m_context;
    bool m_eax;
};