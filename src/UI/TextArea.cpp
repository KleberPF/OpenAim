#include "TextArea.hpp"

#include "ResourceManager.hpp"

using namespace UI;

void TextArea::setText(const char* text, int fontSize)
{
    Text t(&ResourceManager::instance().getFont(LIBERATION), text, fontSize);
    t.color = { .r = 0, .g = 0, .b = 0 };
    auto ptr = std::make_unique<TextRenderable>(t);
    m_textRenderable.swap(ptr);
}

void TextArea::render(const Renderer& renderer) const
{
    renderer.renderRectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h, backgroundColor.toOpenGLFormat());
    if (!m_textRenderable) {
        return;
    }

    const auto& text = m_textRenderable->text();

    float textX = m_rect.x;
    float textY = m_rect.y + m_textRenderable->text().fontSize() - m_textRenderable->text().height() + 3; // arbitrary 3px top margin

    renderer.renderText(*m_textRenderable, textX, textY);
}
