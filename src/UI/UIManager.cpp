#include "UIManager.hpp"

#include "EventManager.hpp"
#include "Events.hpp"
#include "InputManager.hpp"
#include <GLFW/glfw3.h>

#include <cassert>

using namespace UI;

namespace {

std::unique_ptr<UIManager> s_Instance = nullptr;

} // end namespace

void UIManager::init(const Window& window)
{
    if (s_Instance != nullptr) {
        return;
    }

    s_Instance = std::unique_ptr<UIManager>(new UIManager);

    s_Instance->m_viewWidth = window.width;
    s_Instance->m_viewHeight = window.height;
    s_Instance->m_mainWindow = &window;
}

void UIManager::shutdown()
{
    s_Instance.reset();
}

UIManager& UIManager::instance()
{
    assert(s_Instance);
    return *s_Instance;
}

void UIManager::subscribe()
{
    EventManager::instance().addListener(EventType::Resize, [this](EventType type, void* data) {
        onEvent(type, data);
    });
    EventManager::instance().addListener(EventType::MouseButton, [this](EventType type, void* data) {
        onEvent(type, data);
    });
    EventManager::instance().addListener(EventType::CursorPos, [this](EventType type, void* data) {
        onEvent(type, data);
    });
    EventManager::instance().addListener(EventType::KeyPress, [this](EventType type, void* data) {
        onEvent(type, data);
    });
    EventManager::instance().addListener(EventType::Char, [this](EventType type, void* data) {
        onEvent(type, data);
    });
}

Screen* UIManager::addScreen()
{
    auto* screen = new Screen;
    auto ptr = std::unique_ptr<Screen>(screen);
    m_screens.push_back(std::move(ptr));

    return screen;
}

void UIManager::render(Renderer& renderer)
{
    double now = glfwGetTime();
    renderer.orthoProjection = glm::ortho(0.0f, m_viewWidth, m_viewHeight, 0.0f);
    for (auto& screen : m_screens) {
        if (screen->active) {
            screen->render(renderer, now);
        }
    }
}

float UIManager::viewWidth() const
{
    return m_viewWidth;
}

float UIManager::viewHeight() const
{
    return m_viewHeight;
}

void UIManager::setArrowCursor() const
{
    GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    glfwSetCursor(m_mainWindow->ptr(), cursor);
}

void UIManager::setIBeamCursor() const
{
    GLFWcursor* cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    glfwSetCursor(m_mainWindow->ptr(), cursor);
}

void UIManager::handleResize(int width, int height)
{
    m_viewWidth = width;
    m_viewHeight = height;

    for (auto& screen : m_screens) {
        screen->processResize();
    }
}

void UIManager::handleMouseButton(int /*key*/, bool pressed)
{
    for (auto& screen : m_screens) {
        if (screen->active) {
            // TODO: hardcoded LMB
            screen->processClick(MouseButton::BUTTON_LEFT, pressed, m_cursorPos.x, m_cursorPos.y);
        }
    }
}

void UIManager::handleCursorPos(double xpos, double ypos)
{
    m_cursorPos.x = xpos;
    m_cursorPos.y = ypos;

    for (auto& screen : m_screens) {
        if (screen->active) {
            screen->processMouseMove(xpos, ypos);
        }
    }
}

void UIManager::handleKeyPress(int key, bool pressed)
{
    if (pressed) {
        // we only care bout releases for now
        return;
    }

    for (auto& screen : m_screens) {
        if (screen->active) {
            screen->processKeyPress(key);
        }
    }
}

void UIManager::handleChar(unsigned int codepoint)
{
    for (auto& screen : m_screens) {
        if (screen->active) {
            screen->processChar(codepoint);
        }
    }
}

void UIManager::onEvent(EventType type, void* data)
{
    switch (type) {
    case EventType::Resize: {
        auto* event = (ResizeEvent*)data;
        handleResize(event->width, event->height);
    } break;
    case EventType::MouseButton: {
        auto* event = (MouseButtonEvent*)data;
        handleMouseButton(event->button, event->pressed);
    } break;
    case EventType::CursorPos: {
        auto* event = (CursorPosEvent*)data;
        handleCursorPos(event->xpos, event->ypos);
    } break;
    case EventType::KeyPress: {
        auto* event = (KeyPressEvent*)data;
        handleKeyPress(event->key, event->pressed);
    } break;
    case EventType::Char: {
        auto* event = (CharEvent*)data;
        handleChar(event->codepoint);
    } break;
    default:
        break;
    }
}
