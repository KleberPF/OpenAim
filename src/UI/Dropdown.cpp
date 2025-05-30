#include "Dropdown.hpp"

#include "ResourceManager.hpp"

#include <cstddef>

using namespace UI;

void Dropdown::addOption(const char* option)
{
    m_options.emplace_back(Text(&ResourceManager::instance().getFont(LIBERATION), option, 24));
    recreateDropdownRect();
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
        size_t renderedItemCount = 0;

        for (size_t i = 0; i < m_options.size(); i++) {
            if (i == m_selectedIndex) {
                continue;
            }

            renderedItemCount++;
            float yOffset = m_rect.y + renderedItemCount * m_rect.h;
            renderer.renderRectangle(m_rect.x, yOffset, m_rect.w, m_rect.h, backgroundColor.toOpenGLFormat());

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

    
}

void Dropdown::recreateDropdownRect()
{
    // TODO
    if (m_options.size() <= 1) {
        return;
    }

    size_t numItemsInDropdown = m_options.size() - 1;
    m_dropdownRect = {
        .x = m_rect.x,
        .y = m_rect.y + m_rect.h,
        .w = m_rect.w,
        .h = m_rect.h * numItemsInDropdown,
    };
}
