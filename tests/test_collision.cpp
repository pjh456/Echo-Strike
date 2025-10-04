#include <iostream>

#include <echo_strike/collision/collision_manager.hpp>

#include <SDL3/SDL.h>

class Entity;

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    auto window = SDL_CreateWindow("Collision Test", 800, 600, SDL_WINDOW_RESIZABLE);
    auto renderer = SDL_CreateRenderer(window, NULL);

    CollisionManager manager;
    auto &box1 = *manager.create_collision_box();
    box1.set_rect(Rect{100, 100, 200, 200});
    box1.set_enable(true);
    box1.on_collide(
        [](CollisionBox &)
        { std::cout << "Box 1 collided!\n"; });

    auto &box2 = *manager.create_collision_box();
    box2.set_rect(Rect{250, 150, 200, 200});
    box2.set_enable(true);
    box2.on_collide(
        [](CollisionBox &)
        { std::cout << "Box 2 collided!\n"; });

    bool running = true;
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
        }

        manager.process_collide();

        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        // SDL_RenderClear(renderer);

        manager.debug_render(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}