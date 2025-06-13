#pragma once

#include "Color.hpp"
#include "Renderer.hpp"

#include <functional>

namespace UI {

struct Rect {
    float x;
    float y;
    float w;
    float h;

    bool isInside(float _x, float _y) const
    {
        return (_x >= x && _x <= x + w && _y >= y && _y <= y + h);
    }
};

class Widget;

struct ClickEvent {
    bool pressed; // Was the mouse button pressed or released?
    Widget* clicked; // Innermost widget that was clicked
    bool stopped; // Think JS stopPropagation
    double x;
    double y;
};

// Base widget class
// Can be used standalone, usually to render a rectangle
class Widget {
public:
    Widget(Rect relative);
    virtual ~Widget() = default;

    std::function<void(void)> onClick;
    std::function<void(void)> onMouseEnter;
    std::function<void(void)> onMouseLeave;

    Color backgroundColor = { .r = 255, .g = 0, .b = 0 }; // TODO: temp
    bool focusable = false;

    template <typename T>
    T* add(T* widget)
    {
        auto ptr = std::unique_ptr<T>(widget);
        ptr->m_parent = this;
        m_widgets.push_back(std::move(ptr));
        widget->updateRect();
        return widget;
    }

protected:
    virtual void render(const Renderer& renderer) const;
    virtual void updateRect();
    virtual bool isInsideRect(float x, float y);
    virtual void processClick(ClickEvent& event);

    // This rect defines the rect of the widget based on a percentage of the total screen size (0, 1)
    // We then use this rect to calculate the actual pixel rect and update it whenever the screen size changes
    Rect m_relativeRect;
    Rect m_rect;

    std::vector<std::unique_ptr<Widget>> m_widgets;
    Widget* m_parent = nullptr;

    friend class Screen;
};

} // namespace UI
