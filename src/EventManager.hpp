#pragma once

#include <utility>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include <functional>
#include <vector>

class EventManager {
public:
    using ResizeCallback = std::function<void(int, int)>;
    using KeyCallback = std::function<void(int, bool)>;
    using MouseButtonCallback = std::function<void(int, bool)>;
    using CursorPosCallback = std::function<void(double, double)>;

    void addResizeListener(const ResizeCallback& cb);
    void addKeyListener(const KeyCallback& cb);
    void addMouseButtonListener(const MouseButtonCallback& cb);
    void addCursorPosListener(const CursorPosCallback& cb);

private:
    template <typename T, typename... Args>
    void notify(const std::vector<T>& listeners, Args&&... args)
    {
        for (const auto& listener : listeners) {
            listener(std::forward<Args>(args)...);
        }
    }

    // GLFW callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    std::vector<ResizeCallback> m_resizeListeners;
    std::vector<KeyCallback> m_keyListeners;
    std::vector<MouseButtonCallback> m_mouseButtonListeners;
    std::vector<CursorPosCallback> m_cursorPosListeners;

    friend class Window;
};
