#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    auto* windowObj = static_cast<Window*>(glfwGetWindowUserPointer(window));
    windowObj->setWidth(width);
    windowObj->setHeight(height);
}

Window::Window(int width, int height, std::string title, bool fullscreen)
    : m_title(std::move(title))
    , m_fullscreen(fullscreen)
    , m_width(width)
    , m_height(height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    m_ptr = glfwCreateWindow(
        m_width, m_height, m_title.c_str(), monitor, nullptr);
    if (m_ptr == nullptr) {
        std::cerr << "Error creating GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_ptr);
    glfwSwapInterval(0); // turn off vsync
    glfwSetWindowUserPointer(m_ptr, this);
    glfwSetFramebufferSizeCallback(m_ptr, framebufferSizeCallback);

    // glfwSetInputMode(m_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(m_ptr, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    // has to run after glfwCreateWindow
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        exit(EXIT_FAILURE);
    }
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(m_ptr);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(m_ptr);
}

GLFWwindow* Window::getPtr() const
{
    return m_ptr;
}

int Window::getWidth() const
{
    return m_width;
}

void Window::setWidth(int width)
{
    m_width = width;
}

int Window::getHeight() const
{
    return m_height;
}

void Window::setHeight(int height)
{
    m_height = height;
}
