#include "InputManager.hpp"

#include "EventManager.hpp"
#include "Events.hpp"

#include <cassert>
#include <memory>

namespace {

std::unique_ptr<InputManager> s_Instance = nullptr;

} // end namespace

void InputManager::init()
{
    if (s_Instance != nullptr) {
        return;
    }

    s_Instance = std::unique_ptr<InputManager>(new InputManager);
}

void InputManager::shutdown()
{
    s_Instance.reset();
}

InputManager& InputManager::instance()
{
    assert(s_Instance);
    return *s_Instance;
}

bool InputManager::isKeyPressed(Key::Value key)
{
    return m_keys.at(key).current;
}

bool InputManager::isKeyJustPressed(Key::Value key)
{
    return m_keys.at(key).current && !m_keys.at(key).prev;
}

bool InputManager::isMouseButtonPressed(MouseButton::Value button)
{
    return m_mouseBtns.at(button).current;
}

bool InputManager::isMouseButtonJustPressed(MouseButton::Value button)
{
    return m_mouseBtns.at(button).current && !m_mouseBtns.at(button).prev;
}

bool InputManager::didCursorMove() const
{
    return m_cursorMoved;
}

CursorPos InputManager::getCursorPos()
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

void InputManager::subscribe()
{
    EventManager::instance().addListener(EventType::KeyPress, [this](EventType type, void* data) {
        onEvent(type, data);
    });
    EventManager::instance().addListener(EventType::MouseButton, [this](EventType type, void* data) {
        onEvent(type, data);
    });
    EventManager::instance().addListener(EventType::CursorPos, [this](EventType type, void* data) {
        onEvent(type, data);
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
    m_cursorPos.x = xpos;
    m_cursorPos.y = ypos;
}

void InputManager::onEvent(EventType type, void* data)
{
    switch (type) {
    case EventType::KeyPress: {
        auto* event = (KeyPressEvent*)data;
        handleKey(event->key, event->pressed);
    } break;
    case EventType::MouseButton: {
        auto* event = (MouseButtonEvent*)data;
        handleMouseButton(event->button, event->pressed);
    } break;
    case EventType::CursorPos: {
        auto* event = (CursorPosEvent*)data;
        handleCursorPos(event->xpos, event->ypos);
    } break;
    default:
        break;
    }
}
