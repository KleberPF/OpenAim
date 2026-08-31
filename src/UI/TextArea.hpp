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
            std::string currentText = m_textRenderable ? m_textRenderable->text().contents() : "";
            std::string text = std::format("{}{}", currentText, (char)codepoint);

            setText(text.c_str(), 18);
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

            setText(curText.c_str(), 18);
        };
    }

    void setText(const char* text, int fontSize);

private:
    void render(const Renderer& renderer) const override;

    // Ugly, but it's here so we don't have to recreate the text render data every frame
    std::unique_ptr<TextRenderable> m_textRenderable = nullptr;

    friend class Screen;
};

} // namespace UI
