#include "EventManager.hpp"

#include "Events.hpp"

#include <glad/glad.h>

#include <cassert>
#include <memory>
#include <utility>

namespace {

std::unique_ptr<EventManager> s_Instance = nullptr;

} // end namespace

void EventManager::init()
{
    if (s_Instance != nullptr) {
        return;
    }

    s_Instance = std::unique_ptr<EventManager>(new EventManager);
}

void EventManager::shutdown()
{
    s_Instance.reset();
}

EventManager& EventManager::instance()
{
    assert(s_Instance);
    return *s_Instance;
}

void EventManager::addListener(EventType type, const EventHandler& handler)
{
    m_listeners[std::to_underlying(type)].push_back(handler);
}

void EventManager::triggerEvent(EventType type, void* data)
{
    // TODO: this is leaking
    for (auto& handler : m_listeners[std::to_underlying(type)]) {
        handler(type, data);
    }
}

void EventManager::framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
    auto* event = new ResizeEvent {
        .width = width,
        .height = height
    };
    EventManager::instance().triggerEvent(EventType::Resize, event);
}

void EventManager::keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    auto* event = new KeyPressEvent {
        .key = key,
        .pressed = static_cast<bool>(action)
    };
    EventManager::instance().triggerEvent(EventType::KeyPress, event);
}

void EventManager::mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
{
    auto* event = new MouseButtonEvent {
        .button = button,
        .pressed = action != GLFW_RELEASE
    };
    EventManager::instance().triggerEvent(EventType::MouseButton, event);
}

void EventManager::cursorPosCallback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    auto* event = new CursorPosEvent {
        .xpos = xpos,
        .ypos = ypos
    };
    EventManager::instance().triggerEvent(EventType::CursorPos, event);
}
