#pragma once

#include "Camera.hpp"
#include "Entity.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Sprite.hpp"
#include "Weapon.hpp"
#include "Window.hpp"

// settings
constexpr auto SCR_WIDTH = 1920;
constexpr auto SCR_HEIGHT = 1080;
constexpr auto FULLSCREEN = true;
constexpr auto CROSSHAIR_SIZE_PX = 32.0f;

class Game {
public:
    Game();

    void mainLoop();

private:
    void processTiming();
    void processInput();
    void updateEntities();
    void updateShotEntities();
    void render();
    void updateGlfw();

    Window m_window;
    Camera m_camera;
    std::vector<Entity> m_entities;
    std::vector<Sprite> m_sprites;
    std::unique_ptr<Renderer> m_renderer;
    ResourceManager m_resourceManager;
    InputManager m_inputManager;

    Weapon m_weapon;

    // mouse input
    bool m_firstMouse = true;
    float m_lastX;
    float m_lastY;
    float m_mouseSensitivity = 0.03;

    // timing
    float m_deltaTime = 0.0;
    float m_lastFrame = 0.0;

    // objective related stuff
    int m_shotsHit = 0;
    int m_totalShots = 0;

    // game state
    bool m_paused = false;
};

void GLAPIENTRY messageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar* message,
    const void* userParam);