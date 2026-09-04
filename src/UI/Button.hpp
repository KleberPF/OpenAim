#pragma once

#include "UI/Widget.hpp"

namespace UI {

// Because all widgets have a onClick, this is basically the same thing as a Label
class Button : public Widget {
public:
    Button(Rect relative)
        : Widget(relative)
    {
    }

    void setText(const char* text);

private:
    void render(const Renderer& renderer, double time) const override;

    // Ugly, but it's here so we don't have to recreate the text render data every frame
    std::unique_ptr<TextRenderable> m_textRenderable = nullptr;

    friend class Screen;
};

} // namespace UI