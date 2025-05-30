#include "Dropdown.hpp"

#include "ResourceManager.hpp"

#include <cstddef>

using namespace UI;

void Dropdown::addOption(const char* option)
{
    m_options.emplace_back(Text(&ResourceManager::instance().getFont(LIBERATION), option, 24));
    recreateDropdownRect();
}

std::string Dropdown::selectedOption()
{
    return m_options[m_selectedIndex].text().contents();
}

void Dropdown::render(const Renderer& renderer) const
{
    // Main box
    renderer.renderRectangle(m_rect.x, m_rect.y, m_rect.w, m_rect.h, backgroundColor.toOpenGLFormat());

    // Text for the selected item
    auto text = m_options[m_selectedIndex].text();
    float textX = m_rect.x + m_rect.w / 2 - (float)text.width() / 2;
    float textY = m_rect.y + m_rect.h / 2 - (float)text.height() / 2;
    renderer.renderText(m_options[m_selectedIndex], textX, textY);

    // Dropdown options
    if (m_expanded) {
        Color dropdownBackground = backgroundColor + Color { .r = 5, .g = 5, .b = 5 };

        for (size_t i = 0; i < m_options.size(); i++) {
            float yOffset = m_rect.y + (i + 1) * m_rect.h;
            renderer.renderRectangle(m_rect.x, yOffset, m_rect.w, m_rect.h, dropdownBackground.toOpenGLFormat());

            auto text = m_options[i].text();
            float textX = m_rect.x + m_rect.w / 2 - (float)text.width() / 2;
            float textY = yOffset + m_rect.h / 2 - (float)text.height() / 2;
            renderer.renderText(m_options[i], textX, textY);
        }
    }
}

void Dropdown::updateRect(float screenWidth, float screenHeight)
{
    m_rect.x = m_relativeRect.x * screenWidth;
    m_rect.y = m_relativeRect.y * screenHeight;
    m_rect.w = m_relativeRect.w * screenWidth;
    m_rect.h = m_relativeRect.h * screenHeight;

    recreateDropdownRect();
}

bool Dropdown::isInsideRect(float x, float y)
{
    return m_rect.isInside(x, y) || (m_expanded && m_dropdownRect.isInside(x, y));
}

void Dropdown::processClick(float x, float y)
{
    if (m_rect.isInside(x, y)) {
        m_expanded = !m_expanded;
        return;
    }

    if (!m_expanded) {
        return;
    }

    if (m_dropdownRect.isInside(x, y)) {
        // Click inside the dropdown
        // Figure out which option was clicked
        float yOffset = y - m_dropdownRect.y;
        m_selectedIndex = (size_t)(yOffset / m_rect.h);
        m_expanded = false;
    }
}

void Dropdown::recreateDropdownRect()
{
    if (m_options.size() == 0) {
        return;
    }

    m_dropdownRect = {
        .x = m_rect.x,
        .y = m_rect.y + m_rect.h,
        .w = m_rect.w,
        .h = m_rect.h * m_options.size(),
    };
}
