#pragma once

#include "Color.hpp"
#include "Renderer.hpp"

#include <functional>
#include <unordered_set>

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

struct WalkContext {
    Widget* clicked; // Innermost widget that was clicked
    bool stopped; // Think JS stopPropagation (not used for now)
    double x;
    double y;
};

struct HoverWalkContext {
    std::unordered_set<Widget*> hovered; // All widgets that are inside (x, y)
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
    std::function<void(int)> onKeyPressed;

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
    virtual void processClick(float x, float y);

    // Walk through the widget tree to determine which topmost widget was clicked
    void treeWalk(WalkContext& ctx);
    // Walk through the widget tree to determine all widgets which the cursor is hovering
    void treeWalk(HoverWalkContext& ctx);

    // This rect defines the rect of the widget based on a percentage of the total screen size (0, 1)
    // We then use this rect to calculate the actual pixel rect and update it whenever the screen size changes
    Rect m_relativeRect;
    Rect m_rect;

    std::vector<std::unique_ptr<Widget>> m_widgets;
    Widget* m_parent = nullptr;

    friend class Screen;
};

} // namespace UI
