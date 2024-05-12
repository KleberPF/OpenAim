#include "ResourceManager.hpp"

#include "Material.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Sound.hpp"

void ResourceManager::addShader(const std::string& name,
    const std::string& vertexPath, const std::string& fragmentPath)
{
    m_shaders.insert({ name, Shader(vertexPath, fragmentPath) });
}

const Shader& ResourceManager::getShader(const std::string& name)
{
    return m_shaders.at(name);
}

void ResourceManager::addCubemap(
    const std::string& name, const std::array<std::string, 6>& paths)
{
    m_cubemaps.insert({ name, Cubemap(paths) });
}

const Cubemap& ResourceManager::getCubemap(const std::string& name)
{
    return m_cubemaps.at(name);
}

void ResourceManager::addTexture(
    const std::string& name, const std::string& path, Texture::Type type)
{
    m_textures.insert({ name, Texture(path, type) });
}

const Texture& ResourceManager::getTexture(const std::string& name)
{
    return m_textures.at(name);
}

void ResourceManager::addModel(const std::string& name, const std::string& path)
{
    m_models.insert({ name, Model(path) });
}

const Model& ResourceManager::getModel(const std::string& name)
{
    return m_models.at(name);
}

void ResourceManager::addMaterial(const std::string& name)
{
    m_materials.insert({ name, Material() });
}

Material& ResourceManager::getMaterial(const std::string& name)
{
    return m_materials.at(name);
}

void ResourceManager::addSound(const std::string& name, const std::string& path)
{
    m_sounds.emplace_back(name, path);
}

const std::vector<Sound>& ResourceManager::getAllSounds() const
{
    return m_sounds;
}
