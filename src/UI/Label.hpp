#pragma once

#include "UI/Widget.hpp"

namespace UI {

enum class LabelTextAlignment : uint8_t {
    Center,
    Left,
};

class Label : public Widget {
public:
    Label(Rect relative)
        : Widget(relative)
    {
    }

    void setText(const char* text, int fontSize);

    LabelTextAlignment alignment = LabelTextAlignment::Left;

private:
    void render(const Renderer& renderer, double time) const override;

    // Ugly, but it's here so we don't have to recreate the text render data every frame
    std::unique_ptr<TextRenderable> m_textRenderable = nullptr;

    friend class Screen;
};

} // namespace UI
