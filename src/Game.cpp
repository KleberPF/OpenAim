#include "Game.hpp"

#include "Camera.hpp"
#include "Entity.hpp"
#include "EventManager.hpp"
#include "Events.hpp"
#include "Geometry.hpp"
#include "InputManager.hpp"
#include "RNG.hpp"
#include "ResourceManager.hpp"
#include "Scenario.hpp"
#include "Scene.hpp"
#include "SoundPlayer.hpp"
#include "Sprite.hpp"
#include "UI/MainMenu.hpp"
#include "UI/ScenarioOverlay.hpp"
#include "UI/Screen.hpp"
#include "UI/SettingsMenu.hpp"
#include "UI/UIManager.hpp"
#include "Weapon.hpp"
#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include <stb_image.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>

using json = nlohmann::json;

Game::Game()
    : m_window(SCR_WIDTH, SCR_HEIGHT, "OpenAim", FULLSCREEN)
    , m_camera({ 0.0f, 1.5f, 8.0f }, { 0.0, 1.0, 0.0 }, -90.0, 0.0)
    , m_lastX((float)m_window.width / 2)
    , m_lastY((float)m_window.height / 2)
{
    // set/create globals
    EventManager::init();
    RNG::init();
    ResourceManager::init();
    SoundPlayer::init();
    InputManager::init();
    UI::UIManager::init(m_window);

    // set up subscribers to events (resize, mouse move, etc)
    InputManager::instance().subscribe();
    m_window.subscribe();
    UI::UIManager::instance().subscribe();

    Sprite crosshair(ResourceManager::instance().getShader("sprite"),
        ResourceManager::instance().getMaterial("crosshair"),
        { .width = CROSSHAIR_SIZE_PX, .height = CROSSHAIR_SIZE_PX, .relative = false },
        { .x = 0.5f, .y = 0.5f, .relative = true },
        0.0f, glm::vec2(0.5f, 0.5f));
    m_sprites.push_back(crosshair);

    m_globalLightSource.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    m_globalLightSource.ambient = glm::vec3(0.6f, 0.6f, 0.6f);
    m_globalLightSource.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    m_globalLightSource.specular = glm::vec3(1.0f, 1.0f, 1.0f);

    m_skybox = std::make_unique<Skybox>(ResourceManager::instance().getCubemap("skybox"),
        ResourceManager::instance().getShader("skybox"));

    buildPlayArea();
    // parseScenariosFromFile("../resources/scenarios");

    // Build UI (TODO: temp, move this, create a menu manager or something)
    std::vector<std::string> scenarioNames;
    std::ranges::transform(ResourceManager::instance().getAllScenarios(), std::back_inserter(scenarioNames), [](const auto& scenario) {
        return scenario.name;
    });
    m_mainMenu = std::make_unique<UI::MainMenu>(scenarioNames);
    m_settingsMenu = std::make_unique<UI::SettingsMenu>(m_camera.mouseSensitivity);
    m_scenarioOverlay = std::make_unique<UI::ScenarioOverlay>();

    EventManager::instance().addListener(EventType::StartScenario, [this](EventType type, void* data) {
        onEvent(type, data);
    });
    EventManager::instance().addListener(EventType::SettingsUpdate, [this](EventType type, void* data) {
        onEvent(type, data);
    });
}

Game::~Game()
{
    EventManager::shutdown();
    RNG::shutdown();
    ResourceManager::shutdown();
    SoundPlayer::shutdown();
    InputManager::shutdown();
    UI::UIManager::shutdown();
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

    // HACK: needed for Wayland (at least for Sway)
    // Hopefully they merge my PR :)
    // https://github.com/glfw/glfw/pull/2895
    glfwSetCursor(m_window.ptr(), nullptr);
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

    m_shotsFired++;
}

void Game::render()
{
    Scene scene(m_camera, m_window.width, m_window.height);
    scene.globalLightSource = &m_globalLightSource;
    scene.skybox = m_skybox.get();
    scene.entities = &m_entityManager.entities();
    scene.sprites = &m_sprites;
    scene.geometries = &m_geometries;

    // TODO: replace this with a more robust logic for switching screens
    if (m_state == State::Menu) {
        m_settingsMenu->disable();
        m_mainMenu->enable();
    } else if (m_state == State::Paused) {
        m_mainMenu->disable();
        m_settingsMenu->enable();
    } else {
        m_mainMenu->disable();
        m_settingsMenu->disable();
        m_scenarioOverlay->enable();
    }

    m_renderer.renderScene(scene);

    UI::ScenarioOverlayData overlayData = {
        .challenge = m_challengeState.happening,
        .timeRemaining = m_challengeState.timeRemainingSeconds,
        .shotsHit = m_shotsHit,
        .shotsFired = m_shotsFired,
    };

    m_scenarioOverlay->update(overlayData);
    UI::UIManager::instance().render(m_renderer);
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
    Geometry floor(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"), glm::vec3(0));
    floor.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    m_geometries.push_back(floor);

    Geometry frontWall(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"),
        glm::vec3(0.0f, 10.0f, -10.0f));
    frontWall.setRotation(glm::vec3(90, 0, 0));
    frontWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    m_geometries.push_back(frontWall);

    Geometry leftWall(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"),
        glm::vec3(-10.0f, 10.0f, 0.0f));
    leftWall.setRotation(glm::vec3(90, 90, 0));
    leftWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    m_geometries.push_back(leftWall);

    Geometry rightWall(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"), glm::vec3(10.0f, 10.0f, 0.0f));
    rightWall.setRotation(glm::vec3(90, -90, 0));
    rightWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    m_geometries.push_back(rightWall);

    Geometry ceiling(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"), glm::vec3(0.0f, 20.0f, 0.0f));
    ceiling.setRotation(glm::vec3(180, 0, 0));
    ceiling.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    m_geometries.push_back(ceiling);

    Geometry backWall(ResourceManager::instance().getModel("plane"),
        ResourceManager::instance().getMaterial("bricks"),
        ResourceManager::instance().getShader("textured"), glm::vec3(0.0f, 10.0f, 10.0f));
    backWall.setRotation(glm::vec3(90, 180, 0));
    backWall.setSize(glm::vec3(20.0f, 0.0f, 20.0f));
    m_geometries.push_back(backWall);
}

void Game::reset()
{
    m_shotsHit = 0;
    m_shotsFired = 0;
    m_totalTimeSeconds = 0;
    m_challengeState = { };
    m_currentScenario = nullptr;
    m_entityManager.removeAllTargets();
}

void Game::createScenario(const std::string& name)
{
    auto scenario = std::ranges::find_if(ResourceManager::instance().getAllScenarios(), [&name](const auto& scenario) {
        return scenario.name == name;
    });

    if (scenario == ResourceManager::instance().getAllScenarios().end()) {
        // Shouldn't happen so just ignore I guess
        return;
    }

    m_currentScenario = &*scenario;

    m_weapon.type = m_currentScenario->weaponType;
    m_camera.position = glm::vec3(
        m_currentScenario->playerPos.x,
        m_currentScenario->playerPos.y,
        m_currentScenario->playerPos.z);
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
            spawnPoint = glm::vec3(
                target.spawnCoords.x,
                target.spawnCoords.y,
                target.spawnCoords.z);
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

        Geometry geometry(*model, ResourceManager::instance().getMaterial("targets"),
            ResourceManager::instance().getShader("targets"), spawnPoint);
        Entity entity(geometry, spawnPoint);

        entity.addCollisionObject(collisionObjType);
        entity.setSize(target.scale);
        entity.destroyable = true;
        entity.type = target.type;
        entity.setName("Ball " + std::to_string(i));
        entity.setStartingHealth(target.health);
        entity.setMovementPattern(target.positioner);

        m_entityManager.addEntity(std::move(entity));
    }
}

void Game::onEvent(EventType type, void* data)
{
    if (type == EventType::StartScenario) {
        auto* event = (NewScenarioEvent*)data;

        // TODO: use ids?
        createScenario(event->scenario);

        m_challengeState.happening = event->challenge;
        changeState(Game::State::Running);
    } else if (type == EventType::SettingsUpdate) {
        auto* event = (SettingsUpdateEvent*)data;

        m_camera.mouseSensitivity = event->mouseSensitivity;
    }
}
