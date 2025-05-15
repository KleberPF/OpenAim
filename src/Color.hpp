#pragma once

#include <glm/glm.hpp>

struct Color
{
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