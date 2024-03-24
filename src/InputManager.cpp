#include "InputManager.hpp"

#include "GLFW/glfw3.h"

#include <iostream>

std::vector<InputManager*> InputManager::instances_;

InputManager::InputManager(Window& window)
    : window_(window)
{
    InputManager::instances_.push_back(this);
}

bool InputManager::isKeyPressed(const int key)
{
    return this->keys_.at(key).current;
}

bool InputManager::isKeyToggled(int key)
{
    return this->keys_.at(key).current && !this->keys_.at(key).prev;
}

bool InputManager::isMouseButtonPressed(int button)
{
    return this->mouseBtns_.at(button).current;
}

bool InputManager::isMouseButtonToggled(int button)
{
    return this->mouseBtns_.at(button).current &&
           !this->mouseBtns_.at(button).prev;
}

std::pair<float, float> InputManager::getCursorPos()
{
    double xpos = 0;
    double ypos = 0;
    glfwGetCursorPos(this->window_.getPtr(), &xpos, &ypos);
    return {xpos, ypos};
}

void InputManager::consolidateKeyStates()
{
    for (auto& key : this->keys_)
    {
        key.prev = key.current;
    }
    for (auto& button : this->mouseBtns_)
    {
        button.prev = button.current;
    }
}

void InputManager::setupInputCallbacks(GLFWwindow* window)
{
    glfwSetKeyCallback(window, InputManager::keyCallback);
    glfwSetMouseButtonCallback(window, InputManager::mouseButtonCallback);
}

void InputManager::setKeyPressed(int key, bool pressed)
{
    this->keys_.at(key).current = pressed;
}

void InputManager::setMouseButtonPressed(int key, bool pressed)
{
    this->mouseBtns_.at(key).current = pressed;
}

void InputManager::setCursorPos(double xpos, double ypos)
{
    this->cursorPos_.first = xpos;
    this->cursorPos_.second = ypos;
}

void InputManager::keyCallback(GLFWwindow* /*window*/, int key,
                               int /*scancode*/, int action, int /*mods*/)
{
    for (auto* instance : InputManager::instances_)
    {
        instance->setKeyPressed(key, action != GLFW_RELEASE);
    }
}

void InputManager::mouseButtonCallback(GLFWwindow* /*window*/, int button,
                                       int action, int /*mods*/)
{
    for (auto* instance : InputManager::instances_)
    {
        instance->setMouseButtonPressed(button, action != GLFW_RELEASE);
    }
}
