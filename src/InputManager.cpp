#include "InputManager.hpp"

#include <GLFW/glfw3.h>

bool InputManager::isKeyPressed(const int key)
{
    return m_keys.at(key).current;
}

bool InputManager::didCursorMove() const
{
    return m_cursorMoved;
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
    // double xpos = 0;
    // double ypos = 0;
    // glfwGetCursorPos(m_window.ptr(), &xpos, &ypos);
    // return { xpos, ypos };
    return m_cursorPos;
}

void InputManager::consolidateKeyStates()
{
    for (auto& key : m_keys) {
        key.prev = key.current;
    }
    for (auto& button : m_mouseBtns) {
        button.prev = button.current;
    }

    m_cursorMoved = false;
}

void InputManager::subscribe(EventManager& eventManager)
{
    eventManager.addKeyListener([this](int key, bool pressed) {
        handleKey(key, pressed);
    });
    eventManager.addMouseButtonListener([this](int key, bool pressed) {
        handleMouseButton(key, pressed);
    });
    eventManager.addCursorPosListener([this](double xpos, double ypos) {
        handleCursorPos(xpos, ypos);
    });
}

void InputManager::handleKey(int key, bool pressed)
{
    m_keys.at(key).current = pressed;
}

void InputManager::handleMouseButton(int key, bool pressed)
{
    m_mouseBtns.at(key).current = pressed;
}

void InputManager::handleCursorPos(double xpos, double ypos)
{
    m_cursorMoved = true;
    m_cursorPos.first = xpos;
    m_cursorPos.second = ypos;
}
