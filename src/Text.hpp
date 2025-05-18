#pragma once

#include "Color.hpp"
#include "Font.hpp"

#include <string>

class Text {
public:
    Text(const Font* font, const char* text);

    void setFont(const Font* font);
    void setText(const std::string& text);

    const std::string& contents() const;
    int width() const;
    int height() const;

    Color color = { .r = 255, .g = 255, .b = 255 };

private:
    // this is the width in pixels of the text
    // scale is not being account for here
    int m_width = 0;
    // this is the height of the tallest letter
    int m_height = 0;
    std::string m_contents;
    const Font* m_font;

    friend class TextRenderable;
};