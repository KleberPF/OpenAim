#include "UI/Screen.hpp"

#include "InputManager.hpp"

#include <memory>

using namespace UI;

void Screen::processClick(MouseButton::Value button, bool pressed, double xpos, double ypos)
{
    for (auto& widget : m_widgets) {
        if (!widget->m_rect.isInside(xpos, ypos)) {
            continue;
        }

        if (button != MouseButton::BUTTON_LEFT) {
            continue;
        }

        if (pressed) {
            m_clickedWidget = widget.get();
            continue;
        }

        if (m_clickedWidget == widget.get() && widget->onClick) {
            widget->onClick();
        }
    }

    if (!pressed) {
        m_clickedWidget = nullptr;
    }
}

void Screen::processMouseMove(double xpos, double ypos)
{
    bool insideAnyWidget = false;

    for (auto& widget : m_widgets) {
        if (!widget->m_rect.isInside(xpos, ypos)) {
            continue;
        }

        insideAnyWidget = true;

        if (m_hoveredWidget == nullptr) {
            // first frame with mouse inside the widget, trigger onMouseEnter
            m_hoveredWidget = widget.get();
            if (widget->onMouseEnter) {
                widget->onMouseEnter();
            }
        }
    }

    if (!insideAnyWidget) {
        if (m_hoveredWidget != nullptr && m_hoveredWidget->onMouseLeave != nullptr) {
            m_hoveredWidget->onMouseLeave();
        }

        m_hoveredWidget = nullptr;
    }
}

void Screen::processResize(float screenWidth, float screenHeight)
{
    m_viewWidth = screenWidth;
    m_viewHeight = screenHeight;

    for (auto& widget : m_widgets) {
        widget->updateRect(screenWidth, screenHeight);
    }
}

void Screen::render(const Renderer& renderer)
{
    for (auto& widget : m_widgets) {
        widget->render(renderer);
    }
}

Screen::Screen(float screenWidth, float screenHeight)
    : m_viewWidth(screenWidth)
    , m_viewHeight(screenHeight)
{
}
