#ifndef INCLUDE_FLIP_MODE
#define INCLUDE_FLIP_MODE

#include <SDL3/SDL_surface.h>

enum class FlipMode : int
{
    None = SDL_FLIP_NONE,
    Horizontal = SDL_FLIP_HORIZONTAL,
    Vertical = SDL_FLIP_VERTICAL,
    HorizontalAndVectical = SDL_FLIP_HORIZONTAL_AND_VERTICAL
};

#endif // INCLUDE_FLIP_MODE