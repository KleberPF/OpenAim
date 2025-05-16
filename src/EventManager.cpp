#include "EventManager.hpp"

#include <glad/glad.h>

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

void EventManager::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    auto* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
    eventManager->notify(eventManager->m_resizeListeners, width, height);
}

void EventManager::keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
{
    auto* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
    eventManager->notify(eventManager->m_keyListeners, key, action != GLFW_RELEASE);
}

void EventManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/)
{
    auto* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
    eventManager->notify(eventManager->m_mouseButtonListeners, button, action != GLFW_RELEASE);
}

void EventManager::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    auto* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
    eventManager->notify(eventManager->m_cursorPosListeners, xpos, ypos);
}
