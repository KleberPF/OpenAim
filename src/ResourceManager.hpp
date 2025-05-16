#pragma once

// Heavily inspired by the LearnOpenGL version

#include "Font.hpp"
#include "Material.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Sound.hpp"

#include <array>
#include <map>
#include <string>
#include <vector>

class ResourceManager {
public:
    static void init();
    static void shutdown();

    static ResourceManager& instance(); // maybe const this?

    const Shader& getShader(const std::string& name);
    const Cubemap& getCubemap(const std::string& name);
    const Texture& getTexture(const std::string& name);
    const Model& getModel(const std::string& name);
    Material& getMaterial(const std::string& name);
    const std::vector<Sound>& getAllSounds() const;
    const Font& getFont(const std::string& name);

private:
    ResourceManager();

    void addShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    void addCubemap(const std::string& name, const std::array<std::string, 6>& paths);
    void addTexture(const std::string& name, const std::string& path, Texture::Type type);
    void addModel(const std::string& name, const std::string& path);
    void addMaterial(const std::string& name);
    void addSound(const std::string& name, const std::string& path);
    void addFont(const std::string& name, const char* path);

    std::map<std::string, Shader> m_shaders;
    std::map<std::string, Texture> m_textures;
    // This is weird because Cubemap is a Texture, but it's
    // the best way I found to do this for now
    std::map<std::string, Cubemap> m_cubemaps;
    std::map<std::string, Model> m_models;
    std::map<std::string, Material> m_materials;
    std::vector<Sound> m_sounds;
    std::map<std::string, Font> m_fonts;
};