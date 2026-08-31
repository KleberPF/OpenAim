#pragma once

// This class represents the instance of an UI, like the main menu for example.
// A Screen is basically a collection of Widgets and possibly some internal data unique to this UI
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "UI/Widget.hpp"

#include <memory>
#include <unordered_set>
#include <vector>

namespace UI {

class UIManager;

class Screen {
public:
    void processClick(MouseButton::Value button, bool pressed, double xpos, double ypos);
    void processMouseMove(double xpos, double ypos);
    void processResize();
    void processKeyPress(int key);
    void processChar(unsigned int codepoint);

    template <typename T>
    T* add(T* widget)
    {
        m_widgets.push_back(std::unique_ptr<T>(widget));
        widget->updateRect();
        return widget;
    }

    void render(const Renderer& renderer);

    bool active = false;

private:
    Screen() = default;

    Widget* m_clickedWidget = nullptr;
    std::unordered_set<Widget*> m_hoveredWidgets;

    std::vector<std::unique_ptr<Widget>> m_widgets;
    friend class UIManager;
};

} // namespace UI
