#include "Game.hpp"

#include "Camera.hpp"
#include "ClayWrapper.hpp"
#include "Entity.hpp"
#include "InputManager.hpp"
#include "RNG.hpp"
#include "ResourceManager.hpp"
#include "Scenario.hpp"
#include "Scene.hpp"
#include "SoundPlayer.hpp"
#include "Sprite.hpp"
#include "UI/Screen.hpp"
#include "UI/UIManager.hpp"
#include "UI/Widget.hpp"
#include "Weapon.hpp"
#include "Window.hpp"
#include "utils.hpp"

#include <nlohmann/json.hpp>
#include <stb_image.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

using json = nlohmann::json;

Game::Game()
    : m_window(&m_eventManager, SCR_WIDTH, SCR_HEIGHT, "OpenAim", FULLSCREEN)
    , m_camera({ 0.0f, 1.5f, 8.0f }, { 0.0, 1.0, 0.0 }, -90.0, 0.0)
    , m_clayWrapper(m_window.width, m_window.height)
    , m_uiManager(m_window.width, m_window.height)
    , m_lastX((float)m_window.width / 2)
    , m_lastY((float)m_window.height / 2)
{
    // set/create globals
    RNG::init();
    ResourceManager::init();
    SoundPlayer::init();
    InputManager::init();

    // set up subscribers to events (resize, mouse move, etc)
    InputManager::instance().subscribe(m_eventManager);
    m_window.subscribe(m_eventManager);
    m_clayWrapper.subscribe(m_eventManager);
    m_uiManager.subscribe(m_eventManager);

    Sprite crosshair(ResourceManager::instance().getShader("sprite"),
        ResourceManager::instance().getMaterial("crosshair"),
        glm::vec2(CROSSHAIR_SIZE_PX, CROSSHAIR_SIZE_PX),
        glm::vec2(-CROSSHAIR_SIZE_PX / 2, -CROSSHAIR_SIZE_PX / 2), 0.0f);
    m_sprites.push_back(crosshair);

    m_globalLightSource.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    m_globalLightSource.ambient = glm::vec3(0.6f, 0.6f, 0.6f);
    m_globalLightSource.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    m_globalLightSource.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    m_skybox = std::make_unique<Skybox>(ResourceManager::instance().getCubemap("skybox"),
        ResourceManager::instance().getShader("skybox"));

    buildPlayArea();
    parseScenariosFromFile("./resources/scenarios");

    // Build UI (TODO: temp, move this)
    UI::Screen screen;
    screen.active = true;

    UI::Widget widget;
    widget.rect = { .x = 100, .y = 100, .w = 200, .h = 200 };
    widget.onClick = []() {
        std::cout << "Clicked\n";
    };

    screen.addWidget(widget);
    m_uiManager.addScreen(screen);
}

Game::~Game()
{
    RNG::shutdown();
    ResourceManager::shutdown();
    SoundPlayer::shutdown();
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

    if (m_state != Game::State::Running) {
        return;
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
        changeState(Game::State::ChallengeEnded);
        // to prevent rounding issues
        m_challengeState.timeRemainingSeconds = 0;
    }
}

void Game::processInput()
{
    // high priority keys
    if (InputManager::instance().isKeyJustPressed(Key::KEY_F1)) {
        glfwSetWindowShouldClose(m_window.ptr(), true);
        return;
    }

    if (InputManager::instance().isKeyJustPressed(Key::KEY_Y)) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (InputManager::instance().isKeyJustPressed(Key::KEY_ESCAPE)) {
        togglePaused();
    }

    if (m_state != Game::State::Running) {
        glfwSetInputMode(m_window.ptr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        return;
    }

    // mouse input
    if (InputManager::instance().didCursorMove()) {
        auto [xpos, ypos] = InputManager::instance().getCursorPos();

        if (m_ignoreCursorMovement) {
            m_lastX = xpos;
            m_lastY = ypos;
            m_ignoreCursorMovement = false;
        }

        float xoffset = xpos - m_lastX;
        float yoffset = m_lastY - ypos; // reversed since y-coordinates go from bottom to top

        m_lastX = xpos;
        m_lastY = ypos;

        m_camera.processMouseMovement(xoffset, yoffset);
    }

    glfwSetInputMode(m_window.ptr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // camera keyboard processing
    // uncomment this to allow flying around
    if (InputManager::instance().isKeyPressed(Key::KEY_W)) {
        m_camera.processKeyboard(CameraMovement::FORWARD, m_deltaTime);
    }
    if (InputManager::instance().isKeyPressed(Key::KEY_S)) {
        m_camera.processKeyboard(CameraMovement::BACKWARD, m_deltaTime);
    }
    if (InputManager::instance().isKeyPressed(Key::KEY_A)) {
        m_camera.processKeyboard(CameraMovement::LEFT, m_deltaTime);
    }
    if (InputManager::instance().isKeyPressed(Key::KEY_D)) {
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
    if (!InputManager::instance().isMouseButtonPressed(MouseButton::BUTTON_LEFT)) {
        return;
    }

    bool isHoldingMouseButton = !InputManager::instance().isMouseButtonJustPressed(MouseButton::BUTTON_LEFT);
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
    Scene scene(m_camera, m_window.width, m_window.height);
    scene.globalLightSource = m_globalLightSource;
    scene.skybox = *m_skybox;
    scene.entities = m_entityManager.entities();
    scene.sprites = m_sprites;

    m_renderer.renderScene(scene);

    if (m_state == State::Menu) {
        // auto [menuRenderData, scenarioId] = m_clayWrapper.buildMainMenu();
        // m_renderer.renderClayUi(menuRenderData);

        // if (scenarioId.has_value()) {
        //     createScenario(scenarioId.value());
        //     changeState(Game::State::Running);
        // }
        m_uiManager.render(m_renderer);
    }
}

void Game::mainLoopEnd()
{
    InputManager::instance().consolidateKeyStates();
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
        changeState(Game::State::Paused);
    } else if (m_state == Game::State::Paused) {
        changeState(Game::State::Running);
    }
}

void Game::changeState(State newState)
{
    if (newState == m_state) {
        return;
    }

    if (newState == Game::State::Running) {
        // whenever we go from a free cursor back to playing a scenario
        // we need to ignore the first cursor movement to prevent snapping
        m_ignoreCursorMovement = true;
    }

    m_state = newState;
}

void Game::buildPlayArea()
{
    Entity floor(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"), glm::vec3(0));
    floor.addCollisionObject(CollisionObject::Type::AABB);
    floor.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    floor.setName("Floor");
    m_entityManager.addEntity(std::move(floor));

    Entity frontWall(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"),
        glm::vec3(0.0f, 10.0f, -10.0f));
    frontWall.addCollisionObject(CollisionObject::Type::AABB);
    frontWall.setRotation(90, 0, 0);
    frontWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    frontWall.setName("Front Wall");
    m_entityManager.addEntity(std::move(frontWall));

    Entity leftWall(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"),
        glm::vec3(-10.0f, 10.0f, 0.0f));
    leftWall.addCollisionObject(CollisionObject::Type::AABB);
    leftWall.setRotation(90, 90, 0);
    leftWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    leftWall.setName("Left Wall");
    m_entityManager.addEntity(std::move(leftWall));

    Entity rightWall(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"), glm::vec3(10.0f, 10.0f, 0.0f));
    rightWall.addCollisionObject(CollisionObject::Type::AABB);
    rightWall.setRotation(90, -90, 0);
    rightWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    rightWall.setName("Right Wall");
    m_entityManager.addEntity(std::move(rightWall));

    Entity ceiling(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"), glm::vec3(0.0f, 20.0f, 0.0f));
    ceiling.addCollisionObject(CollisionObject::Type::AABB);
    ceiling.setRotation(180, 0, 0);
    ceiling.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    ceiling.setName("Ceiling");
    m_entityManager.addEntity(std::move(ceiling));

    Entity backWall(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"), glm::vec3(0.0f, 10.0f, 10.0f));
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
            std::cout << "Error parsing file " << entry << '\n';
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
                RNG::instance().getFloatInRange(target.minCoords.x, target.maxCoords.x),
                RNG::instance().getFloatInRange(target.minCoords.y, target.maxCoords.y),
                RNG::instance().getFloatInRange(target.minCoords.z, target.maxCoords.z));
        } else {
            spawnPoint = target.spawnCoords;
        }

        const Model* model;
        CollisionObject::Type collisionObjType;
        if (target.shape == Target::Shape::Box) {
            model = &ResourceManager::instance().getModel("cube");
            collisionObjType = CollisionObject::Type::AABB;
        } else {
            model = &ResourceManager::instance().getModel("ball");
            collisionObjType = CollisionObject::Type::SPHERE;
        }

        Entity entity(*model, ResourceManager::instance().getMaterial("targets"),
            ResourceManager::instance().getShader("targets"), spawnPoint);
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
