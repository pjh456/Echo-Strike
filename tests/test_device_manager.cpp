#include <SDL3/SDL.h>

#include <echo_strike/device/device_manager.hpp>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    auto window =
        Window::create()
            .set_title("Device Manager")
            .resizable(true)
            .size(800, 600)
            .build();

    auto renderer =
        window.create_renderer()
            .build();

    auto &manager = DeviceManager::instance();
    manager.set_window(&window);
    manager.set_renderer(&renderer);

    bool running = true;
    SDL_Event e;

    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT)
                running = false;
        }

        renderer.set_draw_color({30, 30, 30});
        renderer.clear();

        renderer.set_draw_color({50, 50, 255});
        renderer.draw_line({500, 200}, {500, 500});

        renderer.set_draw_color({50, 50, 255});
        renderer.draw_rect({100, 100, 50, 50});

        renderer.set_draw_color({255, 50, 50});
        renderer.fill_rect({200, 200, 100, 100});

        renderer.present();
    }

    // SDL_Quit();
    return 0;
}