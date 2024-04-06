#include "ResourceManager.hpp"

#include "Material.hpp"
#include "Model.hpp"
#include "Shader.hpp"

void ResourceManager::addShader(const std::string& name,
                                const std::string& vertexPath,
                                const std::string& fragmentPath)
{
    this->shaders_.insert({name, Shader(vertexPath, fragmentPath)});
}

const Shader& ResourceManager::getShader(const std::string& name)
{
    return this->shaders_.at(name);
}

void ResourceManager::addTexture(const std::string& name,
                                 const std::string& path, Texture::Type type)
{
    this->textures_.insert({name, Texture(path, type)});
}

const Texture& ResourceManager::getTexture(const std::string& name)
{
    return this->textures_.at(name);
}

void ResourceManager::addModel(const std::string& name, const std::string& path,
                               const Material& material, const Shader& shader)
{
    this->models_.insert({name, Model(path, material, shader)});
}

const Model& ResourceManager::getModel(const std::string& name)
{
    return this->models_.at(name);
}

void ResourceManager::addMaterial(const std::string& name)
{
    this->materials_.insert({name, Material()});
}

Material& ResourceManager::getMaterial(const std::string& name)
{
    return this->materials_.at(name);
}
