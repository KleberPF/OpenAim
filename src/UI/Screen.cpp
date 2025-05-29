#include "UI/Screen.hpp"

#include "InputManager.hpp"

using namespace UI;

void Screen::onFrame(const CursorPos& cursorPos, bool clicked)
{
    for (auto& widget : m_widgets) {
        if (!widget.isInsideRect(cursorPos.x, cursorPos.y)) {
            continue;
        }

        if (clicked && widget.onClick) {
            widget.onClick();
        }
    }
}

void Screen::processClick(MouseButton::Value button, bool pressed, double xpos, double ypos)
{
    for (auto& widget : m_widgets) {
        if (!widget.isInsideRect(xpos, ypos)) {
            continue;
        }

        if (button != MouseButton::BUTTON_LEFT) {
            continue;
        }

        if (pressed) {
            m_clickedWidget = &widget;
            continue;
        }

        if (m_clickedWidget == &widget && widget.onClick) {
            widget.onClick();
        }

        m_clickedWidget = nullptr;
    }
}

void Screen::processMouseMove(double xpos, double ypos)
{
    bool insideAnyWidget = false;

    for (auto& widget : m_widgets) {
        if (!widget.isInsideRect(xpos, ypos)) {
            continue;
        }

        insideAnyWidget = true;

        if (m_hoveredWidget == nullptr) {
            // first frame with mouse inside the widget, trigger onMouseEnter
            m_hoveredWidget = &widget;
            if (widget.onMouseEnter) {
                widget.onMouseEnter();
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

void Screen::addWidget(Widget widget)
{
    m_widgets.push_back(std::move(widget));
}

void Screen::render(const Renderer& renderer)
{
    for (auto& widget : m_widgets) {
        widget.render(renderer);
    }
}
