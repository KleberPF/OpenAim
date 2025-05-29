#include "UI/Screen.hpp"

#include "InputManager.hpp"
#include "UIManager.hpp"

#include <memory>

using namespace UI;

void Screen::processClick(MouseButton::Value button, bool pressed, double xpos, double ypos)
{
    for (auto& widget : m_widgets) {
        if (!widget->isInsideRect(xpos, ypos)) {
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
        if (!widget->isInsideRect(xpos, ypos)) {
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
    for (auto& widget : m_widgets) {
        widget->updateRect(screenWidth, screenHeight);
    }
}

Widget* Screen::addWidget(Rect relativeRect)
{
    auto* widget = new Widget(relativeRect, m_parent->m_viewWidth, m_parent->m_viewHeight);
    auto ptr = std::unique_ptr<Widget>(widget);
    m_widgets.push_back(std::move(ptr));

    return widget;
}

void Screen::render(const Renderer& renderer)
{
    for (auto& widget : m_widgets) {
        widget->render(renderer);
    }
}

Screen::Screen(UIManager* parent)
    : m_parent(parent)
{
}
