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

    int getWidth() const;
    int getHeight() const;

    GLFWwindow* getPtr() const;

private:
    GLFWwindow* ptr_;
    std::string title_;
    int width_;
    int height_;
    bool fullscreen_;
};
