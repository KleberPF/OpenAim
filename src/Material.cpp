#include "Material.hpp"

#include <stb_image.h>

Texture::Texture(const std::string& path, Texture::Type type)
    : m_type(type)
{
    glGenTextures(1, &m_id);

    int width = 0;
    int height = 0;
    int nrComponents = 0;
    unsigned char* data
        = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    assert(data != nullptr);

    // default and when nrComponents == 1
    GLenum format = GL_RED;
    if (nrComponents == 3) {
        format = GL_RGB;
    } else if (nrComponents == 4) {
        format = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
    //                 GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, m_id);
}

Texture::Type Texture::getType() const
{
    return m_type;
}

Cubemap::Cubemap(const std::array<std::string, 6>& paths)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    int width = 0;
    int height = 0;
    int nrComponents = 0;

    for (size_t i = 0; i < paths.size(); i++) {
        unsigned char* data
            = stbi_load(paths.at(i).c_str(), &width, &height, &nrComponents, 0);
        assert(data != nullptr);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width,
            height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Cubemap::bind() const
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

Material& Material::setColor(const glm::vec3& color)
{
    m_color = color;
    return *this;
}

Material& Material::setTextureScale(float textureScale)
{
    m_textureScale = textureScale;
    return *this;
}

Material& Material::addTexture(const Texture& texture)
{
    m_textures.emplace_back(texture);
    return *this;
}

void Material::bind(const Shader& shader) const
{
    shader.setVec3("material.color", m_color);
    shader.setFloat("material.shininess", m_shininess);
    shader.setFloat("material.textureScale", m_textureScale);

    const auto textureTypeToStr = [](Texture::Type type) {
        switch (type) {
        case Texture::Type::Diffuse:
            return "material.diffuse";
        case Texture::Type::Specular:
            return "material.specular";
        case Texture::Type::Normal:
            return "material.normal";
        case Texture::Type::Height:
            return "material.height";
        default:
            return "";
        }
    };

    for (size_t i = 0; i < m_textures.size(); i++) {
        const Texture& texture = m_textures[i];
        glActiveTexture(GL_TEXTURE0 + i);

        shader.setInt(textureTypeToStr(texture.getType()), i);
        texture.bind();
    }
}
