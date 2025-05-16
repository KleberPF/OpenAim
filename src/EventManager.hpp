#pragma once

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <functional>
#include <vector>

class EventManager {
public:
    using ResizeCallback = std::function<void(int, int)>;

    void addResizeListener(const ResizeCallback& cb);

private:
    void notifyResize(int width, int height);

    // GLFW callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    std::vector<ResizeCallback> m_resizeListeners;

    friend class Window;
};
