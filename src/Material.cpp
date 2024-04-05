#include "Material.hpp"

#include <stb_image.h>

#include <cassert>
#include <functional>

Texture::Texture(const std::string& path, Texture::Type type)
    : type_(type)
{
    glGenTextures(1, &this->id_);

    int width = 0;
    int height = 0;
    int nrComponents = 0;
    unsigned char* data =
        stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    assert(data != nullptr);

    // default and when nrComponents == 1
    GLenum format = GL_RED;
    if (nrComponents == 3)
    {
        format = GL_RGB;
    }
    else if (nrComponents == 4)
    {
        format = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, this->id_);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, this->id_);
}

Texture::Type Texture::getType() const
{
    return this->type_;
}

void Material::addTexture(const Texture& texture)
{
    this->textures.emplace_back(texture);
}

void Material::bind(const Shader& shader) const
{
    const auto textureTypeToStr = [](Texture::Type type) {
        switch (type)
        {
            case Texture::Type::Diffuse:
                return "diffuse";
            case Texture::Type::Specular:
                return "specular";
            case Texture::Type::Normal:
                return "normal";
            case Texture::Type::Height:
                return "height";
            default:
                return "";
        }
    };

    for (size_t i = 0; i < this->textures.size(); i++)
    {
        const Texture& texture = this->textures[i];
        glActiveTexture(GL_TEXTURE0 + i);

        shader.setInt(textureTypeToStr(texture.getType()), i);
        texture.bind();
    }
}
