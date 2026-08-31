#pragma once

#include "UI/UIManager.hpp"
#include "UI/Widget.hpp"

#include <print>

namespace UI {

class TextArea : public Widget {
public:
    TextArea(Rect relative)
        : Widget(relative)
    {
        backgroundColor = { .r = 255, .g = 255, .b = 255 };
        onMouseEnter = []() {
            std::println("Entered textarea");
            UIManager::instance().setIBeamCursor();
        };
        onMouseLeave = []() {
            std::println("Exited textarea");
            UIManager::instance().setArrowCursor();
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
