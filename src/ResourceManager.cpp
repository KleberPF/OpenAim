#include "ResourceManager.hpp"

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

void ResourceManager::addModel(const std::string& name, const std::string& path)
{
    this->models_.insert({name, Model(path)});
}

const Model& ResourceManager::getModel(const std::string& name)
{
    return this->models_.at(name);
}
