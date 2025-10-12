#include <iostream>
#include <echo_strike/image/animation.hpp>
#include <echo_strike/config/resource_manager.hpp>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <echo_strike/device/window.hpp>
#include <echo_strike/device/renderer.hpp>

#include <chrono>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    Atlas *atlas;
    auto window = Window::create().resizable(true).set_title("animation test").build();
    auto renderer = window.create_renderer().build();

    auto &manager = ResourceManager::instance();

    manager.load_atlases(
        renderer.get_raw(),
        "E:/Projects/games/echo strike/resources",
        "{}.png");

    atlas = ResourceManager::instance().get_atlas("aim").get();
    // std::cout << atlas.load(renderer, "E:/Projects/games/echo strike/resources/enemy/aim/{}.png", 10) << std::endl;
    Animation animation(50);
    animation.add_frames(*atlas);

    auto now = std::chrono::high_resolution_clock::now();

    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer.get_raw(), 0, 128, 255, 255);
        SDL_RenderClear(renderer.get_raw());

        auto cur_time = std::chrono::high_resolution_clock::now();
        auto dur_time = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - now);
        now = cur_time;

        animation.update(dur_time.count());
        animation.render(&renderer);

        SDL_RenderPresent(renderer.get_raw());
    }
}