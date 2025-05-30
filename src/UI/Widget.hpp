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

// Base widget class
// Can be used standalone, usually to render a rectangle
class Widget {
public:
    Widget(Rect relative);
    virtual ~Widget() = default;

    virtual void render(const Renderer& renderer) const;

    std::function<void(void)> onClick;
    std::function<void(void)> onMouseEnter;
    std::function<void(void)> onMouseLeave;

    Color backgroundColor = { .r = 255, .g = 0, .b = 0 }; // TODO: temp
    bool focusable = false;

protected:
    virtual void updateRect(float screenWidth, float screenHeight);

    // This rect defines the rect of the widget based on a percentage of the total screen size (0, 1)
    // We then use this rect to calculate the actual pixel rect and update it whenever the screen size changes
    Rect m_relativeRect;
    Rect m_rect;

    friend class Screen;
};

} // namespace UI
