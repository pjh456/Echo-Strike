#include <echo_strike/image/image.hpp>

#include <SDL3/SDL.h>

Image::Image(SDL_Texture *p_tex)
    : tex(p_tex)
{
    float fw, fh;
    SDL_GetTextureSize(tex, &fw, &fh);
    m_width = static_cast<int>(fw);
    m_height = static_cast<int>(fh);
}

Image::~Image()
{
    if (tex != nullptr)
        SDL_DestroyTexture(tex);
}

Image::Image(Image &&other) noexcept
    : tex(other.tex),
      m_width(other.m_width),
      m_height(other.m_height)
{
    other.tex = nullptr;
}

Image &Image::operator=(Image &&other) noexcept
{
    if (this == &other)
        return *this;

    tex = other.tex, other.tex = nullptr;
    m_width = other.m_width;
    m_height = other.m_height;
    return *this;
}