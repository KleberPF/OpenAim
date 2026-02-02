#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <map>
#include <string>

struct ALSourceAndBuffer {
    ALuint source;
    ALuint buffer;
};

class SoundPlayer {
public:
    ~SoundPlayer();

    static void init();
    static void shutdown();

    static SoundPlayer& instance(); // maybe const this?

    void play(const std::string& soundName);
    void playWithRandomPitch(const std::string& soundName);
    void playIfNotAlreadyPlaying(const std::string& soundName);

private:
    SoundPlayer();

    std::map<std::string, ALSourceAndBuffer> m_sourcesAndBuffers;

    // OpenAL stuff
    ALCdevice* m_device;
    ALCcontext* m_context;
    bool m_eax;
};
