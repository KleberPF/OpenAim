#include "UI/Screen.hpp"

#include "InputManager.hpp"
#include "UI/Widget.hpp"

#include <memory>
#include <unordered_set>

using namespace UI;

void Screen::processClick(MouseButton::Value button, bool pressed, double xpos, double ypos)
{
    if (button != MouseButton::BUTTON_LEFT) {
        return;
    }

    WalkContext ctx = {
        .clicked = nullptr,
        .stopped = false,
        .x = xpos,
        .y = ypos
    };

    // Walk through the widget tree to know which widget the cursor was on
    for (auto& widget : m_widgets) {
        widget->treeWalk(ctx);
    }

    if (pressed) {
        // Mouse button was pressed and we determined which widget it hit
        m_clickedWidget = ctx.clicked;
    } else {
        // Mouse button was released and we need to check if it was a valid click
        if (m_clickedWidget != nullptr && m_clickedWidget == ctx.clicked) {
            m_clickedWidget->processClick(ctx.x, ctx.y);
        }
    }
}

void Screen::processMouseMove(double xpos, double ypos)
{
    HoverWalkContext ctx = {
        .x = xpos,
        .y = ypos
    };

    for (auto& widget : m_widgets) {
        widget->treeWalk(ctx);
    }

    // check the newly hovered widgets (hovered this frame, not hovered in the one before)
    for (auto* widget : ctx.hovered) {
        if (!m_hoveredWidgets.contains(widget) && widget->onMouseEnter) {
            widget->onMouseEnter();
        }
    }

    // check which widgets are not being hovered anymore
    for (auto* widget : m_hoveredWidgets) {
        if (!ctx.hovered.contains(widget) && widget->onMouseLeave) {
            widget->onMouseLeave();
        }
    }

    m_hoveredWidgets = std::move(ctx.hovered);
}

void Screen::processResize()
{
    for (auto& widget : m_widgets) {
        widget->updateRect();
    }
}

void Screen::processChar(unsigned int codepoint)
{
    if (m_clickedWidget && m_clickedWidget->onCharTyped) {
        m_clickedWidget->onCharTyped(codepoint);
    }
}

void Screen::render(const Renderer& renderer)
{
    for (auto& widget : m_widgets) {
        widget->render(renderer);
    }
}
