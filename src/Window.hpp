#pragma once

#include "EventManager.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
    Window(EventManager* eventManager, int width, int height, std::string title, bool fullscreen);

    bool shouldClose();
    void swapBuffers();

    GLFWwindow* ptr() const;

    int width;
    int height;

    void subscribe(EventManager& eventManager);

private:
    void handleResize(int width, int height);

    GLFWwindow* m_ptr;
    std::string m_title;
    bool m_fullscreen;
};
