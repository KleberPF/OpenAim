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

    // shaders
    s_Instance->addShader("color", "./resources/shaders/sprite.vert", "./resources/shaders/color.frag");
    s_Instance->addShader("sprite", "./resources/shaders/sprite.vert", "./resources/shaders/sprite.frag");
    s_Instance->addShader("textured", "./resources/shaders/model.vert", "./resources/shaders/model_lighting.frag");
    s_Instance->addShader("targets", "./resources/shaders/model.vert", "./resources/shaders/model_lighting.frag");
    s_Instance->addShader("skybox", "./resources/shaders/skybox.vert", "./resources/shaders/skybox.frag");
    s_Instance->addShader("healthbar", "./resources/shaders/healthbar.vert", "./resources/shaders/healthbar.frag");
    s_Instance->addShader("text", "./resources/shaders/text.vert", "./resources/shaders/text.frag");

    // fonts
    s_Instance->addFont("liberation", "./resources/fonts/LiberationSans-Regular.ttf");

    // cubemaps
    s_Instance->addCubemap("skybox", { "./resources/textures/skybox/right.bmp", "./resources/textures/skybox/left.bmp", "./resources/textures/skybox/top.bmp", "./resources/textures/skybox/bottom.bmp", "./resources/textures/skybox/front.bmp", "./resources/textures/skybox/back.bmp" });

    // textures
    s_Instance->addTexture("bricks", "./resources/textures/bricks.png", Texture::Type::Diffuse);
    s_Instance->addTexture("crosshair", "./resources/textures/crosshair.png", Texture::Type::Diffuse);
    s_Instance->addTexture("white_pixel", "./resources/textures/white_pixel.png", Texture::Type::Diffuse);

    s_Instance->addMaterial("targets");
    s_Instance->getMaterial("targets").addTexture(s_Instance->getTexture("white_pixel")).setColor(glm::vec3(0.125f, 0.55f, 0.9f));

    s_Instance->addMaterial("bricks");
    s_Instance->getMaterial("bricks").addTexture(s_Instance->getTexture("bricks")).setTextureScale(16);

    s_Instance->addMaterial("crosshair");
    s_Instance->getMaterial("crosshair").addTexture(s_Instance->getTexture("crosshair")).setColor(glm::vec3(0.0f, 1.0f, 0.0f));

    s_Instance->addMaterial("healthbar");
    s_Instance->getMaterial("healthbar").addTexture(s_Instance->getTexture("white_pixel"));

    s_Instance->addModel("cube", "./resources/objects/cube/cube.obj");
    s_Instance->addModel("ball", "./resources/objects/ball/ball.obj");
    s_Instance->addModel("plane", "./resources/objects/plane/plane.obj");

    s_Instance->addSound("pistol", "./resources/sounds/pistol.ogg");
    s_Instance->addSound("machine_gun", "./resources/sounds/machine_gun.ogg");
}

void ResourceManager::shutdown()
{
    s_Instance.reset();
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

void ResourceManager::addFont(const std::string& name, const char* path)
{
    m_fonts.insert({ name, Font(path) });
}

const Font& ResourceManager::getFont(const std::string& name)
{
    return m_fonts.at(name);
}
