#include "InputManager.hpp"

#include "GLFW/glfw3.h"

#include <iostream>

std::vector<InputManager*> InputManager::s_instances;

InputManager::InputManager(Window& window)
    : m_window(window)
{
    InputManager::s_instances.push_back(this);
}

bool InputManager::isKeyPressed(const int key)
{
    return m_keys.at(key).current;
}

bool InputManager::isKeyToggled(int key)
{
    return m_keys.at(key).current && !m_keys.at(key).prev;
}

bool InputManager::isMouseButtonPressed(int button)
{
    return m_mouseBtns.at(button).current;
}

bool InputManager::isMouseButtonToggled(int button)
{
    return m_mouseBtns.at(button).current && !m_mouseBtns.at(button).prev;
}

std::pair<float, float> InputManager::getCursorPos()
{
    double xpos = 0;
    double ypos = 0;
    glfwGetCursorPos(m_window.getPtr(), &xpos, &ypos);
    return { xpos, ypos };
}

void InputManager::consolidateKeyStates()
{
    for (auto& key : m_keys) {
        key.prev = key.current;
    }
    for (auto& button : m_mouseBtns) {
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
    m_keys.at(key).current = pressed;
}

void InputManager::setMouseButtonPressed(int key, bool pressed)
{
    m_mouseBtns.at(key).current = pressed;
}

void InputManager::setCursorPos(double xpos, double ypos)
{
    m_cursorPos.first = xpos;
    m_cursorPos.second = ypos;
}

void InputManager::keyCallback(GLFWwindow* /*window*/, int key,
    int /*scancode*/, int action, int /*mods*/)
{
    for (auto* instance : InputManager::s_instances) {
        instance->setKeyPressed(key, action != GLFW_RELEASE);
    }
}

void InputManager::mouseButtonCallback(GLFWwindow* /*window*/, int button,
    int action, int /*mods*/)
{
    for (auto* instance : InputManager::s_instances) {
        instance->setMouseButtonPressed(button, action != GLFW_RELEASE);
    }
}
