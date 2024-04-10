#pragma once

#include "Shader.hpp"

#include <glad/glad.h>

#include <array>
#include <cstddef>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

class Texture
{
public:
    enum class Type { Diffuse, Specular, Normal, Height, Last };

    Texture(const std::string& path, Texture::Type type);

    void bind() const;

    Texture::Type getType() const;

private:
    GLuint id_;
    Texture::Type type_;
};

class Material
{
public:
    void addTexture(const Texture& texture);
    void bind(const Shader& shader) const;

private:
    std::vector<std::reference_wrapper<const Texture>> textures_;
};