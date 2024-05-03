#include "Sound.hpp"

#include <stb_vorbis.h>

#include <iostream>

Sound::Sound(std::string name, const std::string& path)
    : m_name(std::move(name))
{
    int error = 0;
    stb_vorbis* audioFile = stb_vorbis_open_filename(
        "./resources/sounds/pistol.ogg", &error, nullptr);

    if (!audioFile) {
        std::cerr << "File " << path << "not found\n";
        exit(EXIT_FAILURE);
    }

    stb_vorbis_info info = stb_vorbis_get_info(audioFile);
    m_numChannels = info.channels;
    m_sampleRate = info.sample_rate;
    size_t numSamples = stb_vorbis_stream_length_in_samples(audioFile);
    m_buffer.resize(numSamples * m_numChannels);

    stb_vorbis_get_samples_short_interleaved(
        audioFile, m_numChannels, m_buffer.data(), numSamples * m_numChannels);

    stb_vorbis_close(audioFile);
}

const std::string& Sound::name() const
{
    return m_name;
}

const std::vector<short>& Sound::buffer() const
{
    return m_buffer;
}

int Sound::numChannels() const
{
    return m_numChannels;
}

unsigned int Sound::sampleRate() const
{
    return m_sampleRate;
}
