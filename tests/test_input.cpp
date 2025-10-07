#include <SDL3/SDL.h>

#include <iostream>

#include <echo_strike/device/input_manager.hpp>
#include <echo_strike/device/window.hpp>

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    auto window =
        Window::create()
            .set_title("Input test")
            .size(800, 600)
            .resizable(true)
            .visible(true)
            .build();
    if (!window.valid())
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    auto manager = InputManager::instance();

    bool running = true;
    while (running)
    {
        manager.update();

        // 检查退出条件
        if (manager.is_key_pressed(SDL_SCANCODE_ESCAPE))
            running = false;

        // 检查 WASD 键状态
        if (manager.is_key_pressed(SDL_SCANCODE_W))
            std::cout << "W pressed" << std::endl;
        if (manager.is_key_pressed(SDL_SCANCODE_A))
            std::cout << "A pressed" << std::endl;
        if (manager.is_key_pressed(SDL_SCANCODE_S))
            std::cout << "S pressed" << std::endl;
        if (manager.is_key_pressed(SDL_SCANCODE_D))
            std::cout << "D pressed" << std::endl;

        // 检查鼠标按键
        if (manager.is_mouse_button_pressed(SDL_BUTTON_LEFT))
            std::cout << "Left Mouse pressed" << std::endl;
        if (manager.is_mouse_button_pressed(SDL_BUTTON_RIGHT))
            std::cout << "Right Mouse pressed" << std::endl;

        // 打印鼠标位置
        auto [mx, my] = manager.get_mouse_position();
        std::cout << "Mouse Position: (" << mx << ", " << my << ")" << std::endl;

        SDL_Delay(16); // ~60 FPS
    }

    return 0;
}