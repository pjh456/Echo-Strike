#include <iostream>
#include <echo_strike/image/altas.hpp>
#include <echo_strike/core/timer.hpp>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <chrono>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    Atlas atlas;
    auto window = SDL_CreateWindow("atlas test", 800, 600, 0);
    auto renderer = SDL_CreateRenderer(window, NULL);
    std::cout << atlas.load(renderer, "E:/Projects/games/echo strike/resources/enemy/aim/{}.png", 10) << std::endl;

    size_t atlas_idx = 0;
    Timer t([&atlas_idx]()
            { atlas_idx = atlas_idx == 8 ? 1 : atlas_idx + 1; });
    t.set_timeout_time(50);

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

        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        SDL_RenderClear(renderer);

        auto cur_time = std::chrono::high_resolution_clock::now();
        auto dur_time = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - now);
        now = cur_time;
        t.update(dur_time.count());

        SDL_FRect rect = {200, 150, 400, 300};
        SDL_RenderTexture(renderer, atlas[atlas_idx].get_texture(), NULL, &rect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}