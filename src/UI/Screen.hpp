#pragma once

// This class represents the instance of an UI, like the main menu for example.
// A Screen is basically a collection of Widgets and possibly some internal data unique to this UI
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "UI/Widget.hpp"

#include <memory>
#include <vector>

namespace UI {

class UIManager;

class Screen {
public:
    void processClick(MouseButton::Value button, bool pressed, double xpos, double ypos);
    void processMouseMove(double xpos, double ypos);
    void processResize(float screenWidth, float screenHeight);

    Widget* addWidget(Rect relativeRect);

    void render(const Renderer& renderer);

    bool active;

private:
    Screen(UIManager* parent);

    Widget* m_clickedWidget = nullptr;
    Widget* m_hoveredWidget = nullptr;

    std::vector<std::unique_ptr<Widget>> m_widgets;
    UIManager* m_parent;
    friend class UIManager;
};

} // namespace UI
