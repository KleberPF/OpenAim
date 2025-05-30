#include "UI/Widget.hpp"

using namespace UI;

Widget::Widget(Rect relative)
    : m_relativeRect(relative)
{
}

void Widget::updateRect(float screenWidth, float screenHeight)
{
    m_rect.x = m_relativeRect.x * screenWidth;
    m_rect.y = m_relativeRect.y * screenHeight;
    m_rect.w = m_relativeRect.w * screenWidth;
    m_rect.h = m_relativeRect.h * screenHeight;
}

void Widget::render(const Renderer& renderer) const
{
    renderer.renderRectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h, backgroundColor.toOpenGLFormat());
}

