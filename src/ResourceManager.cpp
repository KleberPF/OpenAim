#include "ResourceManager.hpp"

#include "Material.hpp"
#include "Model.hpp"
#include "Shader.hpp"

void ResourceManager::addShader(const std::string& name,
    const std::string& vertexPath,
    const std::string& fragmentPath)
{
    m_shaders.insert({ name, Shader(vertexPath, fragmentPath) });
}

const Shader& ResourceManager::getShader(const std::string& name)
{
    return m_shaders.at(name);
}

void ResourceManager::addTexture(const std::string& name,
    const std::string& path, Texture::Type type)
{
    m_textures.insert({ name, Texture(path, type) });
}

const Texture& ResourceManager::getTexture(const std::string& name)
{
    return m_textures.at(name);
}

void ResourceManager::addModel(const std::string& name, const std::string& path,
    const Material& material, const Shader& shader)
{
    m_models.insert({ name, Model(path, material, shader) });
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
