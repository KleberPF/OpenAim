#pragma once

#include <glm/glm.hpp>

struct Color {
    int r;
    int g;
    int b;

    inline glm::vec3 toOpenGLFormat() const
    {
        return { (float)r / 255, (float)g / 255, (float)b / 255 };
    }

    inline Color& operator+=(const Color& rhs)
    {
        r += rhs.r;
        g += rhs.g;
        b += rhs.b;
        return *this;
    }

    inline Color& operator-=(const Color& rhs)
    {
        r -= rhs.r;
        g -= rhs.g;
        b -= rhs.b;
        return *this;
    }
};

inline Color operator+(const Color& lhs, const Color& rhs)
{
    Color result = lhs;
    result += rhs;
    return result;
}

inline Color operator-(const Color& lhs, const Color& rhs)
{
    Color result = lhs;
    result -= rhs;
    return result;
}

constexpr Color COLOR_RED = { .r = 255, .g = 0, .b = 0 };
constexpr Color COLOR_GREEN = { .r = 0, .g = 255, .b = 0 };
constexpr Color COLOR_BLUE = { .r = 0, .g = 0, .b = 255 };
constexpr Color COLOR_BLACK = { .r = 0, .g = 0, .b = 0 };
constexpr Color COLOR_WHITE = { .r = 255, .g = 255, .b = 255 };
