#pragma once

#include "EventManager.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
    Window(int width, int height, std::string title, bool fullscreen);

    bool shouldClose();
    void swapBuffers();

    GLFWwindow* ptr() const;

    int width;
    int height;

    void subscribe();

private:
    void handleResize(int width, int height);

    void onEvent(EventType type, void* data);

    GLFWwindow* m_ptr;
    std::string m_title;
    bool m_fullscreen;
};
