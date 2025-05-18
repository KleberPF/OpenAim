#include "ResourceManager.hpp"

#include "Material.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "Sound.hpp"

#include <cassert>
#include <memory>

namespace {

std::unique_ptr<ResourceManager> s_Instance = nullptr;

} // end namespace

void ResourceManager::init()
{
    if (s_Instance != nullptr) {
        return;
    }

    s_Instance = std::unique_ptr<ResourceManager>(new ResourceManager);
}

void ResourceManager::shutdown()
{
    s_Instance.reset();
}

ResourceManager::ResourceManager()
{
    // shaders
    addShader("color", "./resources/shaders/sprite.vert", "./resources/shaders/color.frag");
    addShader("sprite", "./resources/shaders/sprite.vert", "./resources/shaders/sprite.frag");
    addShader("textured", "./resources/shaders/model.vert", "./resources/shaders/model_lighting.frag");
    addShader("targets", "./resources/shaders/model.vert", "./resources/shaders/model_lighting.frag");
    addShader("skybox", "./resources/shaders/skybox.vert", "./resources/shaders/skybox.frag");
    addShader("healthbar", "./resources/shaders/healthbar.vert", "./resources/shaders/healthbar.frag");
    addShader("text", "./resources/shaders/text.vert", "./resources/shaders/text.frag");

    // fonts
    addFont(LIBERATION, Font("./resources/fonts/LiberationSans-Regular.ttf"));

    getFont(LIBERATION)
        .generateGlyphsForFontSize(12)
        .generateGlyphsForFontSize(24)
        .generateGlyphsForFontSize(36);

    // cubemaps
    addCubemap("skybox", { "./resources/textures/skybox/right.bmp", "./resources/textures/skybox/left.bmp", "./resources/textures/skybox/top.bmp", "./resources/textures/skybox/bottom.bmp", "./resources/textures/skybox/front.bmp", "./resources/textures/skybox/back.bmp" });

    // textures
    addTexture("bricks", "./resources/textures/bricks.png", Texture::Type::Diffuse);
    addTexture("crosshair", "./resources/textures/crosshair.png", Texture::Type::Diffuse);
    addTexture("white_pixel", "./resources/textures/white_pixel.png", Texture::Type::Diffuse);

    // materials
    addMaterial("targets");
    getMaterial("targets").addTexture(getTexture("white_pixel")).setColor(glm::vec3(0.125f, 0.55f, 0.9f));

    addMaterial("bricks");
    getMaterial("bricks").addTexture(getTexture("bricks")).setTextureScale(16);

    addMaterial("crosshair");
    getMaterial("crosshair").addTexture(getTexture("crosshair")).setColor(glm::vec3(0.0f, 1.0f, 0.0f));

    addMaterial("healthbar");
    getMaterial("healthbar").addTexture(getTexture("white_pixel"));

    // models
    addModel("cube", "./resources/objects/cube/cube.obj");
    addModel("ball", "./resources/objects/ball/ball.obj");
    addModel("plane", "./resources/objects/plane/plane.obj");

    // sounds
    addSound("pistol", "./resources/sounds/pistol.ogg");
    addSound("machine_gun", "./resources/sounds/machine_gun.ogg");
}

ResourceManager& ResourceManager::instance()
{
    assert(s_Instance);
    return *s_Instance;
}

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

void ResourceManager::addFont(FontId id, Font font)
{
    m_fonts.insert({ id, std::move(font) });
}

Font& ResourceManager::getFont(FontId id)
{
    return m_fonts.at(id);
}
