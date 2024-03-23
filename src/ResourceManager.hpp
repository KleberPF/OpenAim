#pragma once

// Heavily inspired by the LearnOpenGL version, except not a singleton.
// Meant to be instantiated in Game and passed around as reference if needed

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

    void addModel(const std::string& name, const std::string& path);
    const Model& getModel(const std::string& name);

private:
    std::map<std::string, Shader> shaders_;
    std::map<std::string, Model> models_;
};