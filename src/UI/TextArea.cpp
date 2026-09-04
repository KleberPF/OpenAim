#include "TextArea.hpp"

#include "ResourceManager.hpp"

using namespace UI;

void TextArea::setFontSize(int fontSize)
{
    m_fontSize = fontSize;

    // need to reconstruct text object
    setText(m_text);
}

void TextArea::setText(const std::string& text)
{
    Text t(&ResourceManager::instance().getFont(LIBERATION), text, m_fontSize);
    t.color = { .r = 0, .g = 0, .b = 0 };
    auto ptr = std::make_unique<TextRenderable>(t);
    m_textRenderable.swap(ptr);
}

std::string TextArea::text() const
{
    return m_textRenderable ? m_textRenderable->text().contents() : "";
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
