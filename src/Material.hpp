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

class Texture {
public:
    enum class Type { Diffuse, Specular, Normal, Height, Last };

    Texture(const std::string& path, Texture::Type type);
    virtual ~Texture() = default;

    virtual void bind() const;

    Texture::Type getType() const;

protected:
    Texture() = default;

    GLuint m_id;
    Texture::Type m_type;
};

class Cubemap : public Texture {
public:
    Cubemap(const std::array<std::string, 6>& paths);

    void bind() const override;
};

class Material {
public:
    void addTexture(const Texture& texture);
    void bind(const Shader& shader) const;

private:
    std::vector<std::reference_wrapper<const Texture>> m_textures;
};