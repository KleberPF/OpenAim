#pragma once

#include "UI/Widget.hpp"

#include <vector>

namespace UI {

class Dropdown : public Widget {
public:
    Dropdown(Rect relative)
        : Widget(relative)
    {
    }

    void addOption(const char* option);

private:
    void render(const Renderer& renderer) const override;
    void updateRect(float screenWidth, float screenHeight) override;
    bool isInsideRect(float x, float y) override;
    void processClick(float x, float y) override;

    void recreateDropdownRect();

    // Rect for the dropdown part (not relative)
    Rect m_dropdownRect;

    bool m_expanded = false;
    std::vector<TextRenderable> m_options;
    size_t m_selectedIndex = 0;

    friend class Screen;
};

} // namespace UI
