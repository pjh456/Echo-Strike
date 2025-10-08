#ifndef INCLUDE_RENDERER
#define INCLUDE_RENDERER

#include <echo_strike/device/blend_mode.hpp>
#include <echo_strike/device/flip_mode.hpp>
#include <echo_strike/utils/color.hpp>
#include <echo_strike/utils/vec2.hpp>
#include <echo_strike/transform/point.hpp>
#include <echo_strike/transform/rect.hpp>
#include <echo_strike/device/camera2d.hpp>

#include <cstdint>
#include <utility>

struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;

class Window;
class RendererBuilder;
class Camera2D;

class Renderer
{
public:
    static RendererBuilder create();

    enum class VSyncMode : int;
    // enum class ScaleMode : int;
    // enum class LogicalOp : int;
    // enum class RendererLogicalPresentation : int;

private:
    SDL_Renderer *renderer = nullptr;
    SDL_Window *window = nullptr;
    Camera2D *camera = nullptr;

public:
    Renderer(SDL_Window *, SDL_Renderer *);
    Renderer(SDL_Window *, const char * = 0);
    Renderer(SDL_Renderer *);
    Renderer(Window &);
    ~Renderer();

public:
    bool clear() const;
    bool present() const;

    bool set_draw_color(Color) const;

public:
    bool draw_point(const Point &) const;
    bool draw_line(const Point &, const Point &) const;
    bool draw_rect(const Rect &) const;
    bool fill_rect(const Rect &) const;

    bool draw_texture(
        SDL_Texture *,
        const Rect *,
        const Rect * = nullptr,
        double = 0.0,
        FlipMode = static_cast<FlipMode>(0));

    bool set_target(SDL_Texture *texture);
    bool reset_target();

    bool set_viewport(const Rect &);
    bool reset_viewport();

public:
    SDL_Renderer *get_renderer() const { return renderer; }
    void set_renderer(SDL_Renderer *r) { renderer = r; }

    SDL_Window *get_window() const { return window; }
    void set_window(SDL_Window *w) { window = w; }

public:
    VSyncMode get_vsync_mode() const;
    bool set_vsync_mode(VSyncMode);

    BlendMode get_blend_mode() const;
    bool set_blend_mode(BlendMode);

public:
    SDL_Renderer *get_raw() const { return renderer; }
    void set_raw(SDL_Renderer *r) { renderer = r; }

    Camera2D *get_camera() const { return camera; }
    void set_camera(Camera2D *c) { camera = c; }
};

class RendererBuilder
{
private:
    SDL_Window *window = nullptr;
    std::string name;
    Renderer::VSyncMode vsync_flag;

    bool has_vsync_mode = false;
    bool has_name = false;

public:
    RendererBuilder() = default;
    ~RendererBuilder() = default;

    Renderer build()
    {
        auto renderer = Renderer(window, has_name ? name.c_str() : 0);

        if (has_vsync_mode)
            renderer.set_vsync_mode(vsync_flag);

        return renderer;
    }

public:
    RendererBuilder &set_window(Window &w);

    RendererBuilder &set_window(SDL_Window *w)
    {
        window = w;
        return *this;
    }

    RendererBuilder &set_vsync_mode(Renderer::VSyncMode flag)
    {
        vsync_flag = flag;
        return *this;
    }

    RendererBuilder &set_name(const std::string &str)
    {
        name = str, has_name = true;
        return *this;
    }
};

inline RendererBuilder Renderer::create() { return {}; }

#endif // INCLUDE_RENDERER