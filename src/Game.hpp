#pragma once

#include "Camera.hpp"
#include "Entity.hpp"
#include "ResourceManager.hpp"
#include "SpriteRenderer.hpp"
#include "Window.hpp"

#include <memory>

// settings
constexpr auto SCR_WIDTH = 800;
constexpr auto SCR_HEIGHT = 600;
constexpr auto FULLSCREEN = false;
constexpr auto CROSSHAIR_SIZE_PX = 4.0f;

struct MouseButton {
    int prev;
    int current;
};

class Game
{
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

    Window window_;
    Camera camera_;
    std::vector<std::unique_ptr<Entity>> entities_;
    // std::vector<std::unique_ptr<Sprite>> sprites_;
    std::unique_ptr<SpriteRenderer> spriteRenderer_;
    ResourceManager resourceManager_;

    // mouse input
    bool firstMouse_ = true;
    std::array<MouseButton, 2>
        mouseButtons_;  // store either GLFW_RELEASE or GLFW_PRESS
    float lastX_;
    float lastY_;

    // timing
    float deltaTime_ = 0.0;
    float lastFrame_ = 0.0;

    // objective related stuff
    int shotsHit_ = 0;
    int totalShots_ = 0;
};

void GLAPIENTRY messageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam);