#include <iostream>

#include <echo_strike/config/resource_manager.hpp>
#include <echo_strike/image/atlas.hpp>
#include <echo_strike/image/animation.hpp>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <chrono>

int main()
{
    auto &manager = ResourceManager::instance();

    auto file_content = manager.load_resource_str("E:/Projects/games/echo strike/CMakeLists.txt");
    std::cout << *file_content << std::endl;

    SDL_Init(SDL_INIT_VIDEO);

    auto window = SDL_CreateWindow("resource manager test", 800, 600, SDL_WINDOW_RESIZABLE);
    auto renderer = SDL_CreateRenderer(window, NULL);

    auto dir = std::filesystem::path("E:/Projects/games/echo strike/resources");
    manager.set_resource_folder(dir);
    auto atlases = manager.load_atlases(
        renderer,
        dir,
        "{}.png");

    std::cout << "Load "
              << atlases.size()
              << " atlas in the directory."
              << std::endl;
    for (auto atlas : atlases)
        std::cout << "Atlas name: " << atlas->get_name() << std::endl;

    auto [count, atlas] = manager.load_atlas(
        renderer,
        "enemy/aim/{}.png");
    std::cout << "Loaded Atlas's name: " << atlas->get_name() << std::endl;

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

        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        SDL_RenderClear(renderer);

        auto cur_time = std::chrono::high_resolution_clock::now();
        auto dur_time = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - now);
        now = cur_time;

        animation.update(dur_time.count());
        animation.render(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}