#include "UI/Screen.hpp"

#include "InputManager.hpp"
#include "UI/Widget.hpp"

#include <memory>

using namespace UI;

void Screen::processClick(MouseButton::Value button, bool pressed, double xpos, double ypos)
{
    if (button != MouseButton::BUTTON_LEFT) {
        return;
    }

    ClickEvent event = {
        .pressed = pressed,
        .clicked = nullptr,
        .stopped = false,
        .x = xpos,
        .y = ypos
    };

    for (auto& widget : m_widgets) {
        widget->processClick(event);
    }

    if (pressed) {
        // Mouse button was pressed and we determined which widget it hit
        m_clickedWidget = event.clicked;
    } else {
        // Mouse button was released and we need to check if it was a valid click
        if (m_clickedWidget != nullptr && m_clickedWidget == event.clicked && m_clickedWidget->onClick) {
            m_clickedWidget->onClick();
        }

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

void Screen::processResize()
{
    for (auto& widget : m_widgets) {
        widget->updateRect();
    }
}

void Screen::render(const Renderer& renderer)
{
    for (auto& widget : m_widgets) {
        widget->render(renderer);
    }
}
