#pragma once

#include "Shader.hpp"

#include <glad/glad.h>

#include <array>
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
    Material& setColor(const glm::vec3& color);
    Material& setTextureScale(float textureScale);

    Material& addTexture(const Texture& texture);
    void bind(const Shader& shader) const;

private:
    glm::vec3 m_color = glm::vec3(1.0f);
    float m_shininess = 32.0f;
    float m_textureScale = 1.0f;
    std::vector<std::reference_wrapper<const Texture>> m_textures;
};