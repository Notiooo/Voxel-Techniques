#pragma once
#include "GL/gl.h"


/**
 * \brief A struct that represents a color in RGBA format.
 */
struct RGBA
{
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;

    [[nodiscard]] std::array<GLubyte, 4> toArray() const
    {
        return {r, g, b, a};
    }

    bool operator==(const RGBA& other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }
};