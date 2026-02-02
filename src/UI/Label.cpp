#include "Label.hpp"

#include "ResourceManager.hpp"

using namespace UI;

void Label::setText(const char* text)
{
    Text t(&ResourceManager::instance().getFont(LIBERATION), text, 24);
    auto ptr = std::make_unique<TextRenderable>(t);
    m_textRenderable.swap(ptr);
}

void Label::render(const Renderer& renderer) const
{
    renderer.renderRectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h, backgroundColor.toOpenGLFormat());
    if (!m_textRenderable) {
        return;
    }

    auto text = m_textRenderable->text();

    float textX;
    float textY;
    if (alignment == LabelTextAlignment::Center) {
        textX = m_rect.x + m_rect.w / 2 - (float)text.width() / 2;
        textY = m_rect.y + m_rect.h / 2 - (float)text.height() / 2;
    } else {
        textX = m_rect.x;
        textY = m_rect.y + m_rect.h / 2 - (float)text.height() / 2;
    }

    renderer.renderText(*m_textRenderable, textX, textY);
}
