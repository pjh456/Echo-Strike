#ifndef INCLUDE_IMAGE
#define INCLUDE_IMAGE

#include <echo_strike/utils/class_marcos.hpp>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

class Image
{
private:
    SDL_Texture *tex;

    CLASS_READONLY_PROPERTY(int, width)
    CLASS_READONLY_PROPERTY(int, height)

public:
    Image(SDL_Texture *);
    ~Image();

    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;

    Image(Image &&) noexcept;
    Image &operator=(Image &&) noexcept;

public:
    SDL_Texture *get_texture() const { return tex; }
};

#endif // INCLUDE_IMAGE