#include "EventManager.hpp"

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

void EventManager::addResizeListener(const ResizeCallback& cb)
{
    m_resizeListeners.push_back(cb);
}

void EventManager::addKeyListener(const KeyCallback& cb)
{
    m_keyListeners.push_back(cb);
}

void EventManager::addMouseButtonListener(const MouseButtonCallback& cb)
{
    m_mouseButtonListeners.push_back(cb);
}

void EventManager::addCursorPosListener(const CursorPosCallback& cb)
{
    m_cursorPosListeners.push_back(cb);
}

void EventManager::addListener(EventType type, const EventHandler& handler)
{
    m_listeners[std::to_underlying(type)].push_back(handler);
}

void EventManager::triggerEvent(EventType type, void* data)
{
    for (auto& handler : m_listeners[std::to_underlying(type)]) {
        handler(type, data);
    }
}

void EventManager::framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
    EventManager::instance().notify(EventManager::instance().m_resizeListeners, width, height);
}

void EventManager::keyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    EventManager::instance().notify(EventManager::instance().m_keyListeners, key, action != GLFW_RELEASE);
}

void EventManager::mouseButtonCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/)
{
    EventManager::instance().notify(EventManager::instance().m_mouseButtonListeners, button, action != GLFW_RELEASE);
}

void EventManager::cursorPosCallback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    EventManager::instance().notify(EventManager::instance().m_cursorPosListeners, xpos, ypos);
}
