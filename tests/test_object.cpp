#include <iostream>
#include <vector>
#include <random>
#include <cmath>

#include <echo_strike/physics/object.hpp>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    auto window = SDL_CreateWindow("Collision Test", 800, 600, SDL_WINDOW_RESIZABLE);
    auto renderer = SDL_CreateRenderer(window, NULL);

    std::vector<Object> particles;

    bool running = true;
    SDL_Event event;

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> speed_dist(100.0f, 300.0f);
    Uint64 prev_ticks = SDL_GetTicks();

    while (running)
    {
        Uint64 current_ticks = SDL_GetTicks();
        float delta = (current_ticks - prev_ticks) / 1000.0f;
        prev_ticks = current_ticks;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                float mouse_x = static_cast<float>(event.button.x);
                float mouse_y = static_cast<float>(event.button.y);

                Vec2 dir(mouse_x, mouse_y);
                dir = dir.normalize();

                float speed_val = speed_dist(rng);
                Vec2 velocity(dir.get_x() * speed_val, dir.get_y() * speed_val);

                Object p;
                p.set_rect(Rect(0, 0, 25, 25));
                p.set_speed(velocity);
                p.set_force(Vec2(0, 0));

                particles.push_back(p);
            }
        }

        for (auto &p : particles)
            p.on_update(delta);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (auto &p : particles)
            p.render_full(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}