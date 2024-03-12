#include "Window.hpp"

#include "GLFW/glfw3.h"

#include <glad/glad.h>

Window::Window(int width, int height, std::string title, bool fullscreen)
    : width_(width)
    , height_(height)
    , fullscreen_(fullscreen)
    , title_(std::move(title))
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    this->ptr_ = glfwCreateWindow(this->width_, this->height_,
                                  this->title_.c_str(), monitor, nullptr);
    if (this->ptr_ == nullptr)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(this->ptr_);
    glfwSwapInterval(0);  // turn off vsync
    glfwSetFramebufferSizeCallback(this->ptr_, framebufferSizeCallback);

    glfwSetInputMode(this->ptr_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

int Window::getWidth() const
{
    return this->width_;
}

int Window::getHeight() const
{
    return this->height_;
}

GLFWwindow* Window::getPtr() const
{
    return this->ptr_;
}

static void framebufferSizeCallback(GLFWwindow* /*unused*/, int width,
                                    int height)
{
    glViewport(0, 0, width, height);
}
