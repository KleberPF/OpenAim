#include "filereader.hpp"

#include <fstream>
#include <iostream>

//https://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
std::string readTextFile(const std::string& path)
{
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(contents.data(), contents.size());
        in.close();
        return contents;
    }

    std::cerr << "File not found\n";
    throw(errno);
}