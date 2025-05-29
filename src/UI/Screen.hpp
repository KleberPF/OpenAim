#pragma once

// This class represents the instance of an UI, like the main menu for example.
// A Screen is basically a collection of Widgets and possibly some internal data unique to this UI
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "UI/Widget.hpp"

#include <vector>

namespace UI {

class Screen {
public:
    void onFrame(const CursorPos& cursorPos, bool clicked);
    void processClick(MouseButton::Value button, bool pressed, double xpos, double ypos);
    void processMouseMove(double xpos, double ypos);

    void addWidget(Widget widget);

    void render(const Renderer& renderer);

    bool active;

private:
    Widget* m_clickedWidget = nullptr;

    std::vector<Widget> m_widgets;
};

} // namespace UI
