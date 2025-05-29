#pragma once

#include "Color.hpp"
#include "Renderer.hpp"
#include "TextRenderable.hpp"

#include <functional>
#include <memory>

namespace UI {

struct Rect {
    float x;
    float y;
    float w;
    float h;
};

class Widget {
public:
    bool isInsideRect(float x, float y) const;

    void updateRect(float screenWidth, float screenHeight);
    void render(const Renderer& renderer) const;

    void setText(const char* text);

    std::function<void(void)> onClick;
    std::function<void(void)> onMouseEnter;
    std::function<void(void)> onMouseLeave;

    Color backgroundColor = { .r = 255, .g = 0, .b = 0 }; // TODO: temp
    bool focusable = false;

private:
    Widget(Rect relative, float screenWidth, float screenHeight);

    // This rect defines the rect of the widget based on a percentage of the total screen size (0, 1)
    // We then use this rect to calculate the actual pixel rect and update it whenever the screen size changes
    Rect m_relativeRect;
    Rect m_rect;

    // Ugly, but it's here so we don't have to recreate the text render data every frame
    std::unique_ptr<TextRenderable> m_textRenderable = nullptr;

    friend class Screen;
};

} // namespace UI
