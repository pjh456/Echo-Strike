#ifndef INCLUDE_COLOR
#define INCLUDE_COLOR

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
};

#endif // INCLUDE_COLOR