#include "UIManager.hpp"

#include "EventManager.hpp"
#include "Events.hpp"
#include "InputManager.hpp"

#include <cassert>

using namespace UI;

UIManager::UIManager(float viewWidth, float viewHeight)
    : m_viewWidth(viewWidth)
    , m_viewHeight(viewHeight)
{
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
}

Screen* UIManager::addScreen()
{
    auto* screen = new Screen(m_viewWidth, m_viewHeight);
    auto ptr = std::unique_ptr<Screen>(screen);
    m_screens.push_back(std::move(ptr));

    return screen;
}

void UIManager::render(Renderer& renderer)
{
    renderer.orthoProjection = glm::ortho(0.0f, m_viewWidth, m_viewHeight, 0.0f);
    for (auto& screen : m_screens) {
        if (screen->active) {
            screen->render(renderer);
        }
    }
}

void UIManager::handleResize(int width, int height)
{
    m_viewWidth = width;
    m_viewHeight = height;

    for (auto& screen : m_screens) {
        screen->processResize(width, height);
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
    default:
        break;
    }
}
