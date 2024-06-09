#include "Game.hpp"

#include "Camera.hpp"
#include "Entity.hpp"
#include "Globals.hpp"
#include "InputManager.hpp"
#include "Material.hpp"
#include "NuklearWrapper.hpp"
#include "RNG.hpp"
#include "ResourceManager.hpp"
#include "Scenario.hpp"
#include "Scene.hpp"
#include "SoundPlayer.hpp"
#include "Sprite.hpp"
#include "Weapon.hpp"
#include "Window.hpp"
#include "utils.hpp"

#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include <stb_image.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

using json = nlohmann::json;

// globals
RNG* g_rng;
ResourceManager* g_resourceManager;
SoundPlayer* g_soundPlayer;

Game::Game()
    : m_window(SCR_WIDTH, SCR_HEIGHT, "OpenAim", FULLSCREEN)
    , m_camera({ 0.0f, 1.5f, 8.0f }, { 0.0, 1.0, 0.0 }, -90.0, 0.0)
    , m_inputManager(m_window)
    , m_nuklear(m_window.ptr())
    , m_lastX((float)m_window.width / 2)
    , m_lastY((float)m_window.height / 2)
{
    // opengl initialization
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(messageCallback, nullptr);

    InputManager::setupInputCallbacks(m_window.ptr());

    // load shaders and models
    m_resourceManager.addShader("sprite", "./resources/shaders/sprite.vert",
        "./resources/shaders/sprite.frag");
    m_resourceManager.addShader("textured", "./resources/shaders/model.vert",
        "./resources/shaders/model_lighting.frag");
    m_resourceManager.addShader("targets", "./resources/shaders/model.vert",
        "./resources/shaders/model_lighting.frag");
    m_resourceManager.addShader("skybox", "./resources/shaders/skybox.vert",
        "./resources/shaders/skybox.frag");
    m_resourceManager.addShader("healthbar",
        "./resources/shaders/healthbar.vert",
        "./resources/shaders/healthbar.frag");

    m_resourceManager.addCubemap("skybox",
        { "./resources/textures/skybox/right.bmp",
            "./resources/textures/skybox/left.bmp",
            "./resources/textures/skybox/top.bmp",
            "./resources/textures/skybox/bottom.bmp",
            "./resources/textures/skybox/front.bmp",
            "./resources/textures/skybox/back.bmp" });

    m_resourceManager.addTexture(
        "bricks", "./resources/textures/bricks.png", Texture::Type::Diffuse);
    m_resourceManager.addTexture("crosshair",
        "./resources/textures/crosshair.png", Texture::Type::Diffuse);
    m_resourceManager.addTexture("white_pixel",
        "./resources/textures/white_pixel.png", Texture::Type::Diffuse);

    m_resourceManager.addMaterial("targets");
    m_resourceManager.getMaterial("targets")
        .addTexture(m_resourceManager.getTexture("white_pixel"))
        .setColor(glm::vec3(0.125f, 0.55f, 0.9f));

    m_resourceManager.addMaterial("bricks");
    m_resourceManager.getMaterial("bricks")
        .addTexture(m_resourceManager.getTexture("bricks"))
        .setTextureScale(16);

    m_resourceManager.addMaterial("crosshair");
    m_resourceManager.getMaterial("crosshair")
        .addTexture(m_resourceManager.getTexture("crosshair"))
        .setColor(glm::vec3(0.0f, 1.0f, 0.0f));

    m_resourceManager.addMaterial("healthbar");
    m_resourceManager.getMaterial("healthbar")
        .addTexture(m_resourceManager.getTexture("white_pixel"));

    m_resourceManager.addModel("cube", "./resources/objects/cube/cube.obj");
    m_resourceManager.addModel("ball", "./resources/objects/ball/ball.obj");
    m_resourceManager.addModel("plane", "./resources/objects/plane/plane.obj");

    m_resourceManager.addSound("pistol", "./resources/sounds/pistol.ogg");
    m_resourceManager.addSound(
        "machine_gun", "./resources/sounds/machine_gun.ogg");

    // set/create globals
    g_rng = &m_rng;
    g_resourceManager = &m_resourceManager;
    m_soundPlayer = std::make_unique<SoundPlayer>();
    g_soundPlayer = m_soundPlayer.get();

    Sprite crosshair(g_resourceManager->getShader("sprite"),
        g_resourceManager->getMaterial("crosshair"),
        glm::vec2(CROSSHAIR_SIZE_PX, CROSSHAIR_SIZE_PX),
        glm::vec2(-CROSSHAIR_SIZE_PX / 2, -CROSSHAIR_SIZE_PX / 2), 0.0f);
    m_sprites.push_back(crosshair);

    m_globalLightSource.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    m_globalLightSource.ambient = glm::vec3(0.6f, 0.6f, 0.6f);
    m_globalLightSource.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    m_globalLightSource.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    m_skybox = std::make_unique<Skybox>(g_resourceManager->getCubemap("skybox"),
        g_resourceManager->getShader("skybox"));

    buildPlayArea();
    parseScenariosFromFile("./resources/scenarios");
}

void Game::mainLoop()
{
    while (!m_window.shouldClose()) {
        mainLoopBegin();
        processInput();
        updateEntities();
        render();
        mainLoopEnd();
    }
}

void Game::mainLoopBegin()
{
    m_timeNow = (float)glfwGetTime();
    m_deltaTime = m_timeNow - m_lastUpdate;

    if (m_state == Game::State::Running) {
        // whenever we go from a free cursor back to playing a scenario
        // we need to ignore the first cursor movement to prevent snapping
        if (m_prevState != m_state) {
            m_ignoreCursorMovement = true;
        }

        m_totalTimeSeconds += m_deltaTime;
        if (!m_challengeState.happening) {
            return;
        }
        m_challengeState.timeRemainingSeconds -= m_deltaTime;

        if ((m_currentScenario
                && m_currentScenario->winCondition
                    == Scenario::WinCondition::ClearTargets
                && m_entityManager.targetCount() == 0)
            || (m_challengeState.timeRemainingSeconds <= 0)) {
            m_state = Game::State::ChallengeEnded;
            // to prevent rounding issues
            m_challengeState.timeRemainingSeconds = 0;
        }
    }
}

void Game::processInput()
{
    // high priority keys
    if (m_inputManager.isKeyToggled(GLFW_KEY_F1)) {
        glfwSetWindowShouldClose(m_window.ptr(), true);
        return;
    }

    if (m_inputManager.isKeyToggled(GLFW_KEY_Y)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (m_inputManager.isKeyToggled(GLFW_KEY_ESCAPE)) {
        togglePaused();
    }

    if (m_state != Game::State::Running) {
        glfwSetInputMode(m_window.ptr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

    // mouse input
    if (m_inputManager.didCursorMove()) {
        auto [xpos, ypos] = m_inputManager.getCursorPos();

        if (m_ignoreCursorMovement) {
            m_lastX = xpos;
            m_lastY = ypos;
            m_ignoreCursorMovement = false;
        }

        float xoffset = xpos - m_lastX;
        float yoffset = m_lastY
            - ypos; // reversed since y-coordinates go from bottom to top

        m_lastX = xpos;
        m_lastY = ypos;

        m_camera.processMouseMovement(xoffset, yoffset);
    }

    glfwSetInputMode(m_window.ptr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // camera keyboard processing
    // uncomment this to allow flying around
    if (m_inputManager.isKeyPressed(GLFW_KEY_W)) {
        m_camera.processKeyboard(CameraMovement::FORWARD, m_deltaTime);
    }
    if (m_inputManager.isKeyPressed(GLFW_KEY_S)) {
        m_camera.processKeyboard(CameraMovement::BACKWARD, m_deltaTime);
    }
    if (m_inputManager.isKeyPressed(GLFW_KEY_A)) {
        m_camera.processKeyboard(CameraMovement::LEFT, m_deltaTime);
    }
    if (m_inputManager.isKeyPressed(GLFW_KEY_D)) {
        m_camera.processKeyboard(CameraMovement::RIGHT, m_deltaTime);
    }
}

void Game::updateEntities()
{
    if (m_state != Game::State::Running) {
        return;
    }

    updateShotEntities();
    m_entityManager.updateEntities(m_totalTimeSeconds);
}

void Game::updateShotEntities()
{
    if (!m_inputManager.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        return;
    }

    bool isHoldingMouseButton
        = !m_inputManager.isMouseButtonToggled(GLFW_MOUSE_BUTTON_LEFT);
    if (!m_weapon.tryShoot(glfwGetTime() * 1000, isHoldingMouseButton)) {
        return;
    }

    if (m_entityManager.updateShotEntities(
            m_camera.position, m_camera.front())) {
        m_shotsHit++;
    }

    m_totalShots++;
}

void Game::render()
{
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Scene scene(m_camera, m_window.width, m_window.height);
    scene.globalLightSource = m_globalLightSource;
    scene.skybox = *m_skybox;
    scene.entities = m_entityManager.entities();
    scene.sprites = m_sprites;

    m_renderer.renderScene(scene);

    // this signals the beggining of the nuklear rendering when created
    // and the end when destructed (by going out of scope)
    NuklearRenderScope scope;

    if (m_state == Game::State::Menu) {
        std::optional<MenuData> menuData
            = m_nuklear.renderMainMenu(m_scenarios);
        if (menuData.has_value()) {
            createScenario(menuData->scenarioOption);
            m_challengeState.happening = menuData->challenge;
            m_state = Game::State::Running;
        }

        return;
    }

    if (m_state == Game::State::ChallengeEnded) {
        bool ok = m_nuklear.renderChallengeEndStats(m_shotsHit, m_totalShots);
        if (ok) {
            reset();
            m_state = Game::State::Menu;
        }
    } else if (m_challengeState.happening) {
        m_nuklear.renderChallengeData(m_shotsHit, m_totalShots,
            m_challengeState.timeRemainingSeconds,
            1 / (m_timeNow - m_lastFrame));
    } else {
        m_nuklear.renderStats(m_shotsHit, m_totalShots, m_totalTimeSeconds,
            1 / (m_timeNow - m_lastFrame));
    }

    if (m_state == Game::State::Paused) {
        // TODO: probably encapsulate this in the future
        auto settings = m_nuklear.renderPauseMenu();
        if (settings.has_value()) {
            if (settings->sensitivity.has_value()) {
                m_camera.setMouseSensitivity(settings->sensitivity.value());
            }
            if (settings->maxFps.has_value()) {
                float fps = settings->maxFps.value();
                if (fps == 0) {
                    m_fpsCapped = false;
                } else {
                    m_fpsLimit = fps;
                    m_fpsCapped = true;
                }
            }

            g_resourceManager->getMaterial("crosshair")
                .setColor(glm::vec3(settings->crosshairColor.r,
                    settings->crosshairColor.g, settings->crosshairColor.b));
            g_resourceManager->getMaterial("targets").setColor(
                glm::vec3(settings->targetColor.r, settings->targetColor.g,
                    settings->targetColor.b));
        }
    } else {
        // taking input control back from nuklear
        // restore callbacks that were possibly overwritten
        InputManager::setupInputCallbacks(m_window.ptr());
    }
}

void Game::mainLoopEnd()
{
    m_inputManager.consolidateKeyStates();
    glfwPollEvents();

    if (!m_fpsCapped || m_timeNow - m_lastFrame >= 1 / m_fpsLimit) {
        glfwSwapBuffers(m_window.ptr());
        m_lastFrame = m_timeNow;
    }

    m_lastUpdate = m_timeNow;
    m_prevState = m_state;
}

void Game::togglePaused()
{
    if (m_state == Game::State::Running) {
        m_state = Game::State::Paused;
    } else if (m_state == Game::State::Paused) {
        m_state = Game::State::Running;
    }
}

void Game::buildPlayArea()
{
    Entity floor(g_resourceManager->getModel("plane"),
        m_resourceManager.getMaterial("bricks"),
        m_resourceManager.getShader("textured"), glm::vec3(0));
    floor.addCollisionObject(CollisionObject::Type::AABB);
    floor.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    floor.setName("Floor");
    m_entityManager.addEntity(std::move(floor));

    Entity frontWall(g_resourceManager->getModel("plane"),
        m_resourceManager.getMaterial("bricks"),
        m_resourceManager.getShader("textured"),
        glm::vec3(0.0f, 10.0f, -10.0f));
    frontWall.addCollisionObject(CollisionObject::Type::AABB);
    frontWall.setRotation(90, 0, 0);
    frontWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    frontWall.setName("Front Wall");
    m_entityManager.addEntity(std::move(frontWall));

    Entity leftWall(g_resourceManager->getModel("plane"),
        m_resourceManager.getMaterial("bricks"),
        m_resourceManager.getShader("textured"),
        glm::vec3(-10.0f, 10.0f, 0.0f));
    leftWall.addCollisionObject(CollisionObject::Type::AABB);
    leftWall.setRotation(90, 90, 0);
    leftWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    leftWall.setName("Left Wall");
    m_entityManager.addEntity(std::move(leftWall));

    Entity rightWall(g_resourceManager->getModel("plane"),
        m_resourceManager.getMaterial("bricks"),
        m_resourceManager.getShader("textured"), glm::vec3(10.0f, 10.0f, 0.0f));
    rightWall.addCollisionObject(CollisionObject::Type::AABB);
    rightWall.setRotation(90, -90, 0);
    rightWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    rightWall.setName("Right Wall");
    m_entityManager.addEntity(std::move(rightWall));

    Entity ceiling(g_resourceManager->getModel("plane"),
        m_resourceManager.getMaterial("bricks"),
        m_resourceManager.getShader("textured"), glm::vec3(0.0f, 20.0f, 0.0f));
    ceiling.addCollisionObject(CollisionObject::Type::AABB);
    ceiling.setRotation(180, 0, 0);
    ceiling.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    ceiling.setName("Ceiling");
    m_entityManager.addEntity(std::move(ceiling));

    Entity backWall(g_resourceManager->getModel("plane"),
        m_resourceManager.getMaterial("bricks"),
        m_resourceManager.getShader("textured"), glm::vec3(0.0f, 10.0f, 10.0f));
    backWall.addCollisionObject(CollisionObject::Type::AABB);
    backWall.setRotation(90, 180, 0);
    backWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    backWall.setName("Back Wall");
    m_entityManager.addEntity(std::move(backWall));
}

void Game::reset()
{
    m_shotsHit = 0;
    m_totalShots = 0;
    m_totalTimeSeconds = 0;
    m_challengeState = {};
    m_currentScenario = nullptr;
    m_entityManager.removeAllTargets();
}

void Game::parseScenariosFromFile(const std::string& scenarioFolder)
{
    for (const auto& entry :
        std::filesystem::directory_iterator(scenarioFolder)) {
        if (entry.path().extension() != ".json") {
            continue;
        }

        try {
            std::ifstream f(entry.path());
            json data = json::parse(f);

            Scenario scenario;

            std::string filename = entry.path().filename().string();
            scenario.name = filename.substr(0, filename.find('.'));

            std::string weaponType = data["weapon"];

            if (caseInsensitiveEquals(weaponType, "pistol")) {
                scenario.weaponType = Weapon::Type::Pistol;
            } else if (caseInsensitiveEquals(weaponType, "machine_gun")) {
                scenario.weaponType = Weapon::Type::Machine_Gun;
            }

            scenario.playerPos = readVec3FromJSONString(data["playerPos"]);

            if (data.contains("winCondition")
                && caseInsensitiveEquals(
                    data["winCondition"], "cleartargets")) {
                scenario.winCondition = Scenario::WinCondition::ClearTargets;
            }

            scenario.challengeDurationSeconds = data["challengeDuration"];

            auto targets = data["targets"];
            for (auto& target : targets) {
                Target newTarget;

                newTarget.scale = readVec3FromJSONString(target["scale"]);

                std::string shape = target["shape"];
                if (caseInsensitiveEquals(shape, "box")) {
                    newTarget.shape = Target::Shape::Box;
                } else if (caseInsensitiveEquals(shape, "ball")) {
                    newTarget.shape = Target::Shape::Ball;
                }

                newTarget.randomSpawn
                    = target.contains("randomSpawn") && target["randomSpawn"];
                if (newTarget.randomSpawn) {
                    newTarget.minCoords
                        = readVec3FromJSONString(target["minCoords"]);
                    newTarget.maxCoords
                        = readVec3FromJSONString(target["maxCoords"]);
                } else {
                    newTarget.randomSpawn = false;
                    newTarget.spawnCoords
                        = readVec3FromJSONString(target["spawnCoords"]);
                }

                std::string onDestroy = target["onDestroy"];
                if (caseInsensitiveEquals(onDestroy, "move")) {
                    newTarget.type = Entity::Type::MOVER;
                } else {
                    newTarget.type = Entity::Type::GONER;
                }

                if (target.contains("moves")) {
                    newTarget.moves = target["moves"];
                    if (newTarget.moves) {
                        newTarget.movementAmplitude
                            = target["movementAmplitude"];
                        newTarget.movementSpeed = target["movementSpeed"];
                    }
                }

                if (target.contains("health")) {
                    newTarget.health = target["health"];
                }

                scenario.targets.push_back(newTarget);
            }

            m_scenarios.push_back(std::move(scenario));
        } catch (...) {
            // probably some JSON format error
            // just skips the file
        }
    }
}

void Game::createScenario(size_t index)
{
    m_currentScenario = &m_scenarios[index];

    m_weapon.type = m_currentScenario->weaponType;
    m_camera.position = m_currentScenario->playerPos;
    m_camera.lookForward();

    for (size_t i = 0; i < m_currentScenario->targets.size(); i++) {
        const auto& target = m_currentScenario->targets[i];

        glm::vec3 spawnPoint;
        if (target.randomSpawn) {
            spawnPoint = glm::vec3(
                g_rng->getFloatInRange(target.minCoords.x, target.maxCoords.x),
                g_rng->getFloatInRange(target.minCoords.y, target.maxCoords.y),
                g_rng->getFloatInRange(target.minCoords.z, target.maxCoords.z));
        } else {
            spawnPoint = target.spawnCoords;
        }

        const Model* model;
        CollisionObject::Type collisionObjType;
        if (target.shape == Target::Shape::Box) {
            model = &g_resourceManager->getModel("cube");
            collisionObjType = CollisionObject::Type::AABB;
        } else {
            model = &g_resourceManager->getModel("ball");
            collisionObjType = CollisionObject::Type::SPHERE;
        }

        Entity entity(*model, m_resourceManager.getMaterial("targets"),
            m_resourceManager.getShader("targets"), spawnPoint);
        entity.addCollisionObject(collisionObjType);
        entity.setSize(target.scale);
        entity.destroyable = true;
        entity.type = target.type;
        entity.setName("Ball " + std::to_string(i));
        entity.setStartingHealth(target.health);
        if (target.moves) {
            float amplitude = target.movementAmplitude;
            float speed = target.movementSpeed;
            entity.setMovementPattern(
                [amplitude, speed](float timePassedSeconds) {
                    return glm::vec3(
                        amplitude * std::cos(speed * timePassedSeconds), 0.0f,
                        0.0f);
                });
        }
        m_entityManager.addEntity(std::move(entity));
    }
}

void messageCallback(GLenum /*unused*/, GLenum type, GLuint /*unused*/,
    GLenum severity, GLsizei /*unused*/, const GLchar* message,
    const void* /*unused*/)
{
    if (severity == GL_DEBUG_SEVERITY_MEDIUM
        || severity == GL_DEBUG_SEVERITY_HIGH) {
        std::cerr << "GL CALLBACK: "
                  << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "")
                  << " type = 0x" << std::hex << type << ", severity = 0x"
                  << severity << ", message = " << message << std::endl;
    }
}
