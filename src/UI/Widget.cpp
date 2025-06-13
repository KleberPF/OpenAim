#include "UI/Widget.hpp"

#include "UI/UIManager.hpp"

using namespace UI;

Widget::Widget(Rect relative)
    : m_relativeRect(relative)
{
}

void Widget::updateRect()
{
    float parentX = m_parent ? m_parent->m_rect.x : 0;
    float parentY = m_parent ? m_parent->m_rect.y : 0;
    float parentWidth = m_parent ? m_parent->m_rect.w : UIManager::instance().viewWidth();
    float parentHeight = m_parent ? m_parent->m_rect.h : UIManager::instance().viewHeight();

    m_rect.x = parentX + m_relativeRect.x * parentWidth;
    m_rect.y = parentY + m_relativeRect.y * parentHeight;
    m_rect.w = m_relativeRect.w * parentWidth;
    m_rect.h = m_relativeRect.h * parentHeight;

    for (auto& widget : m_widgets) {
        widget->updateRect();
    }
}

void Widget::render(const Renderer& renderer) const
{
    renderer.renderRectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h, backgroundColor.toOpenGLFormat());

    for (const auto& widget : m_widgets) {
        widget->render(renderer);
    }
}

bool Widget::isInsideRect(float x, float y)
{
    return m_rect.isInside(x, y);
}

void Widget::processClick(ClickEvent& event)
{
    if (!isInsideRect(event.x, event.y)) {
        return;
    }

    event.clicked = this;

    for (auto& widget : m_widgets) {
        widget->processClick(event);
    }
}
