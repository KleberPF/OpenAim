#pragma once

// Heavily inspired by the LearnOpenGL version, except not a singleton.
// Meant to be instantiated in Game and passed around as reference if needed

#include "Material.hpp"
#include "Model.hpp"
#include "Shader.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

class ResourceManager
{
public:
    ResourceManager() = default;

    void addShader(const std::string& name, const std::string& vertexPath,
                   const std::string& fragmentPath);
    const Shader& getShader(const std::string& name);

    void addTexture(const std::string& name, const std::string& path,
                    Texture::Type type);
    const Texture& getTexture(const std::string& name);

    void addModel(const std::string& name, const std::string& path,
                  const Material& material);
    const Model& getModel(const std::string& name);

    void addMaterial(const std::string& name);
    Material& getMaterial(const std::string& name);

private:
    std::map<std::string, Shader> shaders_;
    std::map<std::string, Texture> textures_;
    std::map<std::string, Model> models_;
    std::map<std::string, Material> materials_;
};