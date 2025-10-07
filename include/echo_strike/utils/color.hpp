#ifndef INCLUDE_COLOR
#define INCLUDE_COLOR

#include <echo_strike/utils/class_marcos.hpp>

#include <random>

struct Color
{
    int R, G, B, A;

    static Color random_color(bool enable_alpha = false)
    {
        Color color;
        color.R = rand() % 256;
        color.G = rand() % 256;
        color.B = rand() % 256;
        color.A = enable_alpha ? rand() % 256 : 255;
        return color;
    }

    Color(int r, int g, int b, int a = 255)
        : R(r), G(g), B(b), A(a)
    {
    }

    Color(int a = 255) : Color(0, 0, 0, a) {}

    DEFAULT_CONSTRUCTOR(Color)
    DEFAULT_EQUAL_OPERATOR(Color)
};

#endif // INCLUDE_COLOR