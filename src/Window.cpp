#include "Window.hpp"

#include "GLFW/glfw3.h"

#include <glad/glad.h>

#include <iostream>

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    auto* windowObj = static_cast<Window*>(glfwGetWindowUserPointer(window));
    windowObj->width = width;
    windowObj->height = height;
}

Window::Window(int width, int height, std::string title, bool fullscreen)
    : width(width)
    , height(height)
    , title_(std::move(title))
    , fullscreen_(fullscreen)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    this->ptr_ = glfwCreateWindow(this->width, this->height,
                                  this->title_.c_str(), monitor, nullptr);
    if (this->ptr_ == nullptr)
    {
        std::cerr << "Error creating GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(this->ptr_);
    glfwSwapInterval(0);  // turn off vsync
    glfwSetWindowUserPointer(this->ptr_, this);
    glfwSetFramebufferSizeCallback(this->ptr_, framebufferSizeCallback);

    // glfwSetInputMode(this->ptr_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(this->ptr_, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    // has to run after glfwCreateWindow
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        exit(EXIT_FAILURE);
    }
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(this->ptr_);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(this->ptr_);
}

GLFWwindow* Window::getPtr() const
{
    return this->ptr_;
}
