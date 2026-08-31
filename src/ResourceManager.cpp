#include "ResourceManager.hpp"

#include "Material.hpp"
#include "Model.hpp"
#include "Scenario.hpp"
#include "Shader.hpp"
#include "Sound.hpp"
#include "Weapon.hpp"

#include <cassert>
#include <filesystem>
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
    addShader("color", "../resources/shaders/sprite.vert", "../resources/shaders/color.frag");
    addShader("sprite", "../resources/shaders/sprite.vert", "../resources/shaders/sprite.frag");
    addShader("textured", "../resources/shaders/model.vert", "../resources/shaders/model_lighting.frag");
    addShader("targets", "../resources/shaders/model.vert", "../resources/shaders/model_lighting.frag");
    addShader("skybox", "../resources/shaders/skybox.vert", "../resources/shaders/skybox.frag");
    addShader("healthbar", "../resources/shaders/healthbar.vert", "../resources/shaders/healthbar.frag");
    addShader("text", "../resources/shaders/text.vert", "../resources/shaders/text.frag");

    // fonts
    addFont(LIBERATION, Font("../resources/fonts/LiberationSans-Regular.ttf"));

    getFont(LIBERATION)
        .generateGlyphsForFontSize(12)
        .generateGlyphsForFontSize(18)
        .generateGlyphsForFontSize(24)
        .generateGlyphsForFontSize(36);

    // cubemaps
    addCubemap("skybox", { "../resources/textures/skybox/right.bmp", "../resources/textures/skybox/left.bmp", "../resources/textures/skybox/top.bmp", "../resources/textures/skybox/bottom.bmp", "../resources/textures/skybox/front.bmp", "../resources/textures/skybox/back.bmp" });

    // textures
    addTexture("bricks", "../resources/textures/bricks.png", Texture::Type::Diffuse);
    addTexture("crosshair", "../resources/textures/crosshair.png", Texture::Type::Diffuse);
    addTexture("white_pixel", "../resources/textures/white_pixel.png", Texture::Type::Diffuse);

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
    addModel("cube", "../resources/objects/cube/cube.obj");
    addModel("ball", "../resources/objects/ball/ball.obj");
    addModel("plane", "../resources/objects/plane/plane.obj");

    // sounds
    addSound("pistol", "../resources/sounds/pistol.ogg");
    addSound("machine_gun", "../resources/sounds/machine_gun.ogg");

    // scenarios
    loadScenarios("../resources/scenarios");
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

void ResourceManager::loadScenarios(const std::string& path)
{
    m_lua.open_libraries(sol::lib::base, sol::lib::math);

    // init "enums" used in the lua definition of a scenario
    std::string enums = R"(
        Weapon = {
            Pistol = 0,
            Machine_Gun = 1,
        }
        WinCondition = {
            ClearTargets = 0,
            Time = 1,
        }
        Shape = {
            Box = 0,
            Ball = 1,
        }
        Type = {
            Mover = 0,
            Goner = 1,
        }
    )";
    m_lua.script(enums);

    auto coordFromTable = [&](const sol::table& table) {
        return Scenario::Coordinate {
            .x = table["x"],
            .y = table["y"],
            .z = table["z"],
        };
    };

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() != ".lua") {
            continue;
        }

        sol::table ret = m_lua.script_file(entry.path());
        if (!ret.valid()) {
            throw std::invalid_argument("Error when parsing .lua scenario file");
        }

        Scenario scenario;
        scenario.name = entry.path().filename().string();

        if (!ret["weapon"].valid()) {
            throw std::invalid_argument("Scenario has no weapon defined");
        }

        scenario.weaponType = static_cast<Weapon::Type>(ret["weapon"]);

        if (!ret["player_pos"].valid()) {
            throw std::invalid_argument("Scenario has no player_pos defined");
        }
        scenario.playerPos = coordFromTable(ret["player_pos"]);

        if (ret["win_condition"].valid()) {
            scenario.winCondition = static_cast<Scenario::WinCondition>(ret["win_condition"]);
        }

        if (!ret["challenge_duration"].valid()) {
            throw std::invalid_argument("Scenario has no challenge_duration defined");
        }
        scenario.challengeDurationSeconds = ret["challenge_duration"];

        sol::table targets = ret["targets"];
        if (!targets.valid()) {
            throw std::invalid_argument("Scenario has no targets defined");
        }

        int a = targets.size();

        targets.for_each([&](const sol::object&, const sol::object& value) {
            sol::table luaTarget = value;

            if (!luaTarget.valid()) {
                throw std::invalid_argument("Some target is invalid");
            }

            Target target;

            if (!luaTarget["scale"].valid()) {
                throw std::invalid_argument("Target has no scale defined");
            }
            if (luaTarget["scale"].is<double>()) {
                target.scale.x = target.scale.y = target.scale.z = luaTarget["scale"];
            } else {
                target.scale.x = luaTarget["scale"]["x"];
                target.scale.y = luaTarget["scale"]["y"];
                target.scale.z = luaTarget["scale"]["z"];
            }

            if (!luaTarget["shape"].valid()) {
                throw std::invalid_argument("Target has no shape defined");
            }
            target.shape = static_cast<Target::Shape>(luaTarget["shape"]);

            // TODO: can we assume luaTarget["random_spawn"] is considered as a bool here?
            target.randomSpawn = luaTarget["random_spawn"].valid() && luaTarget["random_spawn"];
            if (target.randomSpawn) {
                if (!luaTarget["min_coords"].valid()) {
                    throw std::invalid_argument("Target with random spawn needs min_coords set");
                }
                target.minCoords = coordFromTable(luaTarget["min_coords"]);

                if (!luaTarget["max_coords"].valid()) {
                    throw std::invalid_argument("Target with random spawn needs max_coords set");
                }
                target.maxCoords = coordFromTable(luaTarget["max_coords"]);
            } else {
                if (!luaTarget["spawn_coords"].valid()) {
                    throw std::invalid_argument("Target without random spawn needs spawn_coords set");
                }
                target.spawnCoords = coordFromTable(luaTarget["spawn_coords"]);
            }

            if (!luaTarget["type"].valid()) {
                throw std::invalid_argument("Target needs a type");
            }
            target.type = static_cast<Target::Type>(luaTarget["type"]);

            if (luaTarget["positioner"].valid()) {
                // This is ugly but I can't find a clean way of doing this
                sol::function positioner = luaTarget["positioner"];
                sol::table curPosTable = m_lua.create_table();
                sol::table refPosTable = m_lua.create_table();

                target.positioner = [curPosTable, refPosTable, positioner](Scenario::Coordinate refPos, Scenario::Coordinate curPos, double d) mutable {
                    curPosTable["x"] = curPos.x;
                    curPosTable["y"] = curPos.y;
                    curPosTable["z"] = curPos.z;

                    refPosTable["x"] = refPos.x;
                    refPosTable["y"] = refPos.y;
                    refPosTable["z"] = refPos.z;

                    sol::table ret = positioner(refPosTable, curPosTable, d);

                    return Scenario::Coordinate {
                        .x = ret["x"],
                        .y = ret["y"],
                        .z = ret["z"],
                    };
                };
            }

            if (luaTarget["health"].valid()) {
                target.health = luaTarget["health"];
            }

            scenario.targets.push_back(target);
        });

        m_scenarios.push_back(scenario);
    }
}

const std::vector<Scenario>& ResourceManager::getAllScenarios() const
{
    return m_scenarios;
}
