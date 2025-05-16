#include "EventManager.hpp"

#include <glad/glad.h>

void EventManager::addResizeListener(const ResizeCallback& cb)
{
    m_resizeListeners.push_back(cb);
}

void EventManager::notifyResize(int width, int height)
{
    for (auto& listener : m_resizeListeners) {
        listener(width, height);
    }
}

void EventManager::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    auto* eventManager = static_cast<EventManager*>(glfwGetWindowUserPointer(window));
    eventManager->notifyResize(width, height);
}
