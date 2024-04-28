#pragma once

#include <string>
#include <vector>

class Sound {
public:
    Sound(std::string name, const std::string& path);

    const std::string& name() const;
    const std::vector<short>& buffer() const;
    int numChannels() const;
    unsigned int sampleRate() const;

private:
    std::string m_name;
    std::vector<short> m_buffer;
    int m_numChannels;
    unsigned int m_sampleRate;
};
