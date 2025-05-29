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
};

class Widget {
public:
    bool isInsideRect(float x, float y) const;

    void render(const Renderer& renderer) const;

    std::function<void(void)> onHover;
    std::function<void(void)> onClick;

    Rect rect;
    Color backgroundColor = { .r = 255, .g = 0, .b = 0 }; // TODO: temp
    bool focusable = false;

private:
};

} // namespace UI
