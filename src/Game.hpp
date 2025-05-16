#pragma once

#include "Camera.hpp"
#include "ClayWrapper.hpp"
#include "EntityManager.hpp"
#include "InputManager.hpp"
#include "RNG.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Scenario.hpp"
#include "SoundPlayer.hpp"
#include "Sprite.hpp"
#include "Weapon.hpp"
#include "Window.hpp"

// settings
constexpr auto SCR_WIDTH = 800;
constexpr auto SCR_HEIGHT = 600;
constexpr auto FULLSCREEN = false;
constexpr auto CROSSHAIR_SIZE_PX = 32.0f;
constexpr auto CHALLENGE_DURATION = 30.0f;

struct ChallengeState {
    bool happening = false;
    float timeRemainingSeconds = CHALLENGE_DURATION;
};

class Game {
public:
    enum class State {
        Menu,
        Paused,
        ChallengeEnded,
        Running,
    };

    Game();
    ~Game();

    void mainLoop();

private:
    void mainLoopBegin();
    void processInput();
    void updateEntities();
    void updateShotEntities();
    void render();
    void mainLoopEnd();

    void togglePaused();
    void changeState(State newState);

    void buildPlayArea();
    void reset();

    void parseScenariosFromFile(const std::string& scenarioFolder);
    void createScenario(size_t index);

    Window m_window;
    Camera m_camera;
    EntityManager m_entityManager;
    std::vector<Sprite> m_sprites;
    Renderer m_renderer;
    InputManager m_inputManager;
    LightSource m_globalLightSource;
    std::unique_ptr<Skybox> m_skybox;
    Weapon m_weapon;
    std::vector<Scenario> m_scenarios;
    Scenario* m_currentScenario = nullptr;
    ClayWrapper m_clayWrapper;

    // mouse input
    // This is meant to be set every time we go from a free moving cursor to
    // one locked in the middle of the screen.
    bool m_ignoreCursorMovement = true;
    float m_lastX;
    float m_lastY;

    // timing
    // Time elapsed since the app started running
    // without considering time spent paused
    float m_totalTimeSeconds = 0.0f;
    float m_deltaTime = 0.0f;
    float m_lastFrame = 0.0f;
    float m_timeNow = 0.0f;
    float m_lastUpdate = 0.0f;
    bool m_fpsCapped = true;
    float m_fpsLimit = 300.0f;

    // objective related stuff
    int m_shotsHit = 0;
    int m_totalShots = 0;

    // challenge tracking
    ChallengeState m_challengeState;

    // game state
    State m_state = State::Menu;
    State m_prevState = State::Menu;
};

void GLAPIENTRY messageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar* message,
    const void* userParam);
