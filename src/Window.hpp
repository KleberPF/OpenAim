#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>

class Window
{
public:
    Window(int width, int height, std::string title, bool fullscreen);

    bool shouldClose();
    void swapBuffers();

    GLFWwindow* getPtr() const;

    int width;
    int height;

private:
    GLFWwindow* ptr_;
    std::string title_;
    bool fullscreen_;
};
