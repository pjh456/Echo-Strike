#include <echo_strike/device/renderer.hpp>

#include <echo_strike/device/window.hpp>
#include <echo_strike/utils/vec2.hpp>
#include <echo_strike/utils/rect.hpp>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_blendmode.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>

enum class Renderer::VSyncMode : int
{
    Disabled = SDL_RENDERER_VSYNC_DISABLED,
    Enabled = 1,
    Adaptive = SDL_RENDERER_VSYNC_ADAPTIVE
};

enum class Renderer::BlendMode : uint32_t
{
    None = SDL_BLENDMODE_NONE,
    Blend = SDL_BLENDMODE_BLEND,
    BlendPremutiplied = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
    Add = SDL_BLENDMODE_ADD,
    AddPremutiplied = SDL_BLENDMODE_ADD_PREMULTIPLIED,
    Mod = SDL_BLENDMODE_MOD,
    Mul = SDL_BLENDMODE_MUL,
    Invalid = SDL_BLENDMODE_INVALID
};

enum class Renderer::FlipMode : int
{
    None = SDL_FLIP_NONE,
    Horizontal = SDL_FLIP_HORIZONTAL,
    Vertical = SDL_FLIP_VERTICAL,
    HorizontalAndVectical = SDL_FLIP_HORIZONTAL_AND_VERTICAL
};

// enum class Renderer::ScaleMode : int
// {
//     Invalid = SDL_SCALEMODE_INVALID,
//     Nearest = SDL_SCALEMODE_NEAREST,
//     Linear = SDL_SCALEMODE_LINEAR,
//     PixelArt = SDL_SCALEMODE_PIXELART
// };

// enum class Renderer::RendererLogicalPresentation : int
// {
//     Disabled = SDL_LOGICAL_PRESENTATION_DISABLED,
//     Stretch = SDL_LOGICAL_PRESENTATION_STRETCH,
//     LetterBox = SDL_LOGICAL_PRESENTATION_LETTERBOX,
//     OverScan = SDL_LOGICAL_PRESENTATION_OVERSCAN,
//     IntegerScale = SDL_LOGICAL_PRESENTATION_INTEGER_SCALE
// };

Renderer::Renderer(SDL_Window *w, SDL_Renderer *r)
    : window(w),
      renderer(r)
{
}

Renderer::Renderer(SDL_Window *w, const char *name)
    : Renderer(w, SDL_CreateRenderer(w, name))
{
}

Renderer::Renderer(SDL_Renderer *r)
    : Renderer(SDL_GetRenderWindow(r), r)
{
}

Renderer::Renderer(Window &w)
    : Renderer(w.get_raw())
{
}

Renderer::~Renderer()
{
    if (renderer)
        SDL_DestroyRenderer(renderer);

    renderer = nullptr;
}

bool Renderer::clear() const
{
    if (!renderer)
        return false;

    return SDL_RenderClear(renderer);
}

bool Renderer::present() const
{
    if (!renderer)
        return false;

    return SDL_RenderPresent(renderer);
}

bool Renderer::set_draw_color(Color color) const
{
    if (!renderer)
        return false;

    return SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
}

bool Renderer::draw_point(const Vec2 &pos) const
{
    if (!renderer)
        return false;

    return SDL_RenderPoint(renderer, pos.get_x(), pos.get_y());
}

bool Renderer::draw_line(const Vec2 &from, const Vec2 &to) const
{
    if (!renderer)
        return false;

    return SDL_RenderLine(renderer, from.get_x(), from.get_y(), to.get_x(), to.get_y());
}

bool Renderer::draw_rect(const Rect &rect) const
{
    if (!renderer)
        return false;

    auto frect = rect.to_frect();
    return SDL_RenderRect(renderer, &frect);
}

bool Renderer::fill_rect(const Rect &rect) const
{
    if (!renderer)
        return false;

    auto frect = rect.to_frect();
    return SDL_RenderFillRect(renderer, &frect);
}

bool Renderer::draw_texture(
    SDL_Texture *texture,
    const Rect *dst,
    const Rect *src,
    double angle,
    FlipMode flip)
{
    if (!renderer)
        return false;

    auto dst_frect = dst->to_frect();
    auto src_frect = src->to_frect();

    return SDL_RenderTextureRotated(
        renderer,
        texture,
        &src_frect,
        &dst_frect,
        angle,
        nullptr,
        static_cast<SDL_FlipMode>(flip));
}

bool Renderer::set_target(SDL_Texture *texture)
{
    if (!renderer)
        return false;

    return SDL_SetRenderTarget(renderer, texture);
}

bool Renderer::reset_target()
{
    if (!renderer)
        return false;

    return SDL_SetRenderTarget(renderer, nullptr);
}

bool Renderer::set_viewport(const Rect &rect)
{
    if (!renderer)
        return false;

    auto sdl_rect = rect.to_rect();
    return SDL_SetRenderViewport(renderer, &sdl_rect);
}

bool Renderer::reset_viewport()
{
    if (!renderer)
        return false;

    return SDL_SetRenderViewport(renderer, nullptr);
}

Renderer::VSyncMode Renderer::get_vsync_mode() const
{
    int flag;
    SDL_GetRenderVSync(renderer, &flag);

    switch (flag)
    {
    case SDL_RENDERER_VSYNC_DISABLED:
        return VSyncMode::Disabled;
    case SDL_RENDERER_VSYNC_ADAPTIVE:
        return VSyncMode::Adaptive;
    default:
        return VSyncMode::Enabled;
    }
}

bool Renderer::set_vsync_mode(VSyncMode flag)
{
    if (!renderer)
        return false;

    return SDL_SetRenderVSync(renderer, static_cast<int>(flag));
}

Renderer::BlendMode Renderer::get_blend_mode() const
{
    SDL_BlendMode flag;
    SDL_GetRenderDrawBlendMode(renderer, &flag);

    switch (flag)
    {
    case SDL_BLENDMODE_NONE:
        return BlendMode::None;
    case SDL_BLENDMODE_BLEND:
        return BlendMode::Blend;
    case SDL_BLENDMODE_BLEND_PREMULTIPLIED:
        return BlendMode::BlendPremutiplied;
    case SDL_BLENDMODE_ADD:
        return BlendMode::Add;
    case SDL_BLENDMODE_ADD_PREMULTIPLIED:
        return BlendMode::AddPremutiplied;
    case SDL_BLENDMODE_MOD:
        return BlendMode::Mod;
    case SDL_BLENDMODE_MUL:
        return BlendMode::Mul;
    case SDL_BLENDMODE_INVALID:
    default:
        return BlendMode::Invalid;
    }
}

bool Renderer::set_blend_mode(BlendMode flag)
{
    if (!renderer)
        return false;

    return SDL_SetRenderDrawBlendMode(renderer, static_cast<SDL_BlendMode>(flag));
}

RendererBuilder &RendererBuilder::set_window(Window &w)
{
    window = w.get_raw();
    return *this;
}