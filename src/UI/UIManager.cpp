#include "UIManager.hpp"

#include "InputManager.hpp"

#include <cassert>

using namespace UI;

UIManager::UIManager(float viewWidth, float viewHeight)
    : m_viewWidth(viewWidth)
    , m_viewHeight(viewHeight)
{
}

void UIManager::subscribe(EventManager& eventManager)
{
    eventManager.addResizeListener([this](int width, int height) {
        handleResize(width, height);
    });
    eventManager.addMouseButtonListener([this](int key, bool pressed) {
        handleMouseButton(key, pressed);
    });
    eventManager.addCursorPosListener([this](double xpos, double ypos) {
        handleCursorPos(xpos, ypos);
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
