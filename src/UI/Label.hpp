#pragma once

#include "UI/Widget.hpp"

namespace UI {

class Label : public Widget {
public:
    Label(Rect relative)
        : Widget(relative)
    {
    }

    void setText(const char* text);

private:
    void render(const Renderer& renderer) const override;

    // Ugly, but it's here so we don't have to recreate the text render data every frame
    std::unique_ptr<TextRenderable> m_textRenderable = nullptr;

    friend class Screen;
};

} // namespace UI