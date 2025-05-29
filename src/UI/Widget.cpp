#include "UI/Widget.hpp"

using namespace UI;

bool Widget::isInsideRect(float x, float y) const
{
    return (
        x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h);
}

void Widget::render(const Renderer& renderer) const
{
    renderer.renderRectangle(rect.x, rect.y, rect.w, rect.h, backgroundColor.toOpenGLFormat());
}
