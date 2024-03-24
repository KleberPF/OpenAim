#pragma once
// based on https://stackoverflow.com/questions/55573238/how-do-i-do-a-proper-input-class-in-glfw-for-a-game-engine

#include "Window.hpp"

#include <GLFW/glfw3.h>

#include <array>
#include <map>
#include <utility>
#include <vector>

// This struct holds information about a keyboard key
// or mouse button. prev is the state of the key on the previous
// frame, and current is the state in the current frame (pressed
// or not pressed). prev is generally used to know if a button was
// held between multiple frames and prevent spam
struct InputState {
    bool prev = false;
    bool current = false;
};

class InputManager
{
public:
    InputManager(Window& window);

    bool isKeyPressed(int key);

    // Returns wheter a key is being pressed this frame
    // but wasn't on the previous one
    bool isKeyToggled(int key);

    bool isMouseButtonPressed(int button);

    // Same idea as isKeyToggled
    bool isMouseButtonToggled(int button);

    std::pair<float, float> getCursorPos();

    // Should be called after all input processing to update
    // the keys that were pressed this frame
    void consolidateKeyStates();

    // MUST be called after all instances are created,
    // otherwise the callback won't be set
    static void setupInputCallbacks(GLFWwindow* window);

private:
    Window& window_;
    std::array<InputState, GLFW_KEY_LAST + 1> keys_;
    std::array<InputState, GLFW_MOUSE_BUTTON_LAST + 1> mouseBtns_;
    std::pair<double, double> cursorPos_ = {-1, -1};
    static std::vector<InputManager*> instances_;

    void setKeyPressed(int key, bool pressed);
    void setMouseButtonPressed(int key, bool pressed);
    void setCursorPos(double xpos, double ypos);

    static void keyCallback(GLFWwindow* window, int key, int scancode,
                            int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action,
                                    int mods);
};
