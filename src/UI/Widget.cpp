#include "UI/Widget.hpp"
#include "ResourceManager.hpp"
#include "TextRenderable.hpp"
#include <memory>

using namespace UI;

bool Widget::isInsideRect(float x, float y) const
{
    return (x >= m_rect.x && x <= m_rect.x + m_rect.w && y >= m_rect.y && y <= m_rect.y + m_rect.h);
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
    if (m_textRenderable) {
        // Center text inside widget
        // TODO: figure out how to make this configurable
        auto text = m_textRenderable->text();
        float textX = m_rect.x + m_rect.w / 2 - (float)text.width() / 2;
        float textY = m_rect.y + m_rect.h / 2 - (float)text.height() / 2;
        renderer.renderText(*m_textRenderable, textX, textY);
    }
}

void Widget::setText(const char* text)
{
    Text t(&ResourceManager::instance().getFont(LIBERATION), text, 24);
    auto ptr = std::make_unique<TextRenderable>(t);
    m_textRenderable.swap(ptr);
}

Widget::Widget(Rect relative, float screenWidth, float screenHeight)
    : m_relativeRect(relative)
{
    updateRect(screenWidth, screenHeight);
}
