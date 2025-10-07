#ifndef INCLUDE_WINDOW
#define INCLUDE_WINDOW

#include <cstdint>
#include <string>
#include <utility>

struct SDL_Window;

class Renderer;
class RendererBuilder;
class WindowBuilder;

class Window
{
public:
    static WindowBuilder create();
    RendererBuilder create_renderer();

private:
    SDL_Window *window = nullptr;

public:
    Window(SDL_Window *);
    Window(const std::string &, int, int, uint32_t = 0);
    Window();
    ~Window();

public:
    bool show();
    bool hide();

    bool valid() const { return window != nullptr; }

public:
    SDL_Window *get_raw() const { return window; }
    void set_raw(SDL_Window *w) { window = w; }

public:
    bool
    resize(int, int);
    std::pair<int, int> get_size();

    bool set_position(int, int);
    std::pair<int, int> get_position();

    bool set_title(const std::string &);
    std::string get_title();

    uint32_t get_flags();
};

class WindowBuilder
{
private:
    Window *window = nullptr;

    std::string title = "";
    int width = 800, height = 600;
    uint32_t flags = 0;

public:
    WindowBuilder() = default;
    ~WindowBuilder() = default;

    Window build() { return Window(title.c_str(), width, height, flags); }

public:
    WindowBuilder &size(int w, int h)
    {
        width = w, height = h;
        return *this;
    }

    WindowBuilder &set_title(const std::string &t)
    {
        title = t;
        return *this;
    }

public:
    WindowBuilder &resizable(bool);
    WindowBuilder &visible(bool);
};

inline WindowBuilder Window::create() { return {}; }

#endif // INCLUDE_WINDOW