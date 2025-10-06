#include <echo_strike/device/window.hpp>

#include <SDL3/SDL_video.h>

Window::Window(SDL_Window *w)
    : window(w)
{
}

Window::Window(
    const std::string &title,
    int width,
    int height,
    uint32_t flags)
    : Window(SDL_CreateWindow(title.c_str(), width, height, flags))
{
}

Window::Window()
    : Window("SDL Window", 800, 600)
{
}

Window::~Window()
{
    if (window)
        SDL_DestroyWindow(window);
    window = nullptr;
}

bool Window::show()
{
    if (!window)
        return false;

    return SDL_ShowWindow(window);
}

bool Window::hide()
{
    if (!window)
        return false;

    return SDL_HideWindow(window);
}

bool Window::resize(int w, int h)
{
    if (!window)
        return false;

    return SDL_SetWindowSize(window, w, h);
}

std::pair<int, int> Window::get_size()
{
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    return {width, height};
}

bool Window::set_position(int x, int y)
{
    if (!window)
        return false;
    return SDL_SetWindowPosition(window, x, y);
}

std::pair<int, int> Window::get_position()
{
    int x, y;
    SDL_GetWindowPosition(window, &x, &y);
    return {x, y};
}

bool Window::set_title(const std::string &title)
{
    if (!window)
        return false;
    return SDL_SetWindowTitle(window, title.c_str());
}

std::string Window::get_title()
{
    if (!window)
        return "";
    return SDL_GetWindowTitle(window);
}

uint32_t Window::get_flags()
{
    if (!window)
        return 0;
    return SDL_GetWindowFlags(window);
}

WindowBuilder &WindowBuilder::resizable(bool flag)
{
    if (flag)
        flags |= SDL_WINDOW_RESIZABLE;
    else
        flags &= ~SDL_WINDOW_RESIZABLE;
    return *this;
}

WindowBuilder &WindowBuilder::visible(bool flag)
{
    if (flag)
        flags &= ~SDL_WINDOW_HIDDEN;
    else
        flags |= SDL_WINDOW_HIDDEN;
    return *this;
}