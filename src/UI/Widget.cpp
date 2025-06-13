#include "UI/Widget.hpp"

#include "UI/UIManager.hpp"

using namespace UI;

Widget::Widget(Rect relative)
    : m_relativeRect(relative)
{
}

void Widget::updateRect()
{
    float screenWidth = UIManager::instance().viewWidth();
    float screenHeight = UIManager::instance().viewHeight();

    m_rect.x = m_relativeRect.x * screenWidth;
    m_rect.y = m_relativeRect.y * screenHeight;
    m_rect.w = m_relativeRect.w * screenWidth;
    m_rect.h = m_relativeRect.h * screenHeight;
}

void Widget::render(const Renderer& renderer) const
{
    renderer.renderRectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h, backgroundColor.toOpenGLFormat());
}

bool Widget::isInsideRect(float x, float y)
{
    return m_rect.isInside(x, y);
}

void Widget::processClick(float /*x*/, float /*y*/)
{
    if (onClick) {
        onClick();
    }
}
