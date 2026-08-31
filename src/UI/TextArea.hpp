#pragma once

#include "UI/UIManager.hpp"
#include "UI/Widget.hpp"

#include <format>

namespace UI {

class TextArea : public Widget {
public:
    TextArea(Rect relative)
        : Widget(relative)
    {
        backgroundColor = { .r = 255, .g = 255, .b = 255 };
        onMouseEnter = []() {
            UIManager::instance().setIBeamCursor();
        };
        onMouseLeave = []() {
            UIManager::instance().setArrowCursor();
        };
        onCharTyped = [&](unsigned int codepoint) {
            // TODO: bad, improve this
            // this requires setText("", ...) being called before using the text area
            std::string text = std::format("{}{}", m_textRenderable->text().contents(), (char)codepoint);
            if (validator && !validator(text)) {
                // update rejected
                return;
            }

            setText(text.c_str(), m_textRenderable->text().fontSize());
        };
        onKeyPressed = [&](int key) {
            if (key != GLFW_KEY_BACKSPACE) {
                return;
            }

            if (!m_textRenderable || m_textRenderable->text().contents().length() == 0) {
                return;
            }

            std::string curText = m_textRenderable->text().contents();
            curText.pop_back();

            if (validator && !validator(curText)) {
                // update rejected
                return;
            }
            setText(curText.c_str(), m_textRenderable->text().fontSize());
        };
    }

    void setText(const char* text, int fontSize);
    std::string text() const;

private:
    void render(const Renderer& renderer) const override;

    // Ugly, but it's here so we don't have to recreate the text render data every frame
    std::unique_ptr<TextRenderable> m_textRenderable = nullptr;

    friend class Screen;
};

} // namespace UI
