#include "SoundPlayer.hpp"

#include "Globals.hpp"

#include <AL/al.h>
#include <stb_vorbis.h>

#include <cstdlib>
#include <ios>
#include <iostream>
#include <memory>
#include <vector>

SoundPlayer::SoundPlayer()
    : m_device(alcOpenDevice(nullptr))
{
    if (!m_device) {
        std::cerr << "Failed to open ALC device\n";
        exit(EXIT_FAILURE);
    }

    m_context = alcCreateContext(m_device, nullptr);
    alcMakeContextCurrent(m_context);

    m_eax = alIsExtensionPresent("EAX2.0");
    alGetError(); // clear error code

    auto sounds = g_resourceManager->getAllSounds();
    for (auto& sound : sounds) {
        ALuint buffer = 0;
        alGenBuffers(1, &buffer);

        ALenum format
            = sound.numChannels() == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

        alBufferData(buffer, format, sound.buffer().data(),
            sound.buffer().size() * sizeof(short), sound.sampleRate());

        ALuint source = 0;
        alGenSources(1, &source);
        alSourcei(source, AL_BUFFER, buffer);
        // for now we just hardcode this to make the sounds not REALLY loud
        // could be a parameter in the play functions later if needed
        alSourcef(source, AL_GAIN, 0.1f);

        m_sources.insert({ sound.name(), source });
    }

    if (ALenum error = alGetError(); error != AL_NO_ERROR) {
        std::cerr << "Some error happened when creating SoundPlayer\n";
        exit(EXIT_FAILURE);
    }
}

SoundPlayer::~SoundPlayer()
{
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(m_context);
    alcCloseDevice(m_device);
}

void SoundPlayer::play(const std::string& soundName)
{
    ALuint source = m_sources.at(soundName);
    alSourcef(source, AL_PITCH, 1);
    alSourcePlay(source);
}

void SoundPlayer::playWithRandomPitch(const std::string& soundName)
{
    ALuint source = m_sources.at(soundName);
    alSourcef(source, AL_PITCH, g_rng->getFloatInRange(0.9f, 1.1f));
    alSourcePlay(source);
}

void SoundPlayer::playIfNotAlreadyPlaying(const std::string& soundName)
{
    ALuint source = m_sources.at(soundName);
    ALint state = 0;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) {
        return;
    }
    alSourcePlay(source);
}
