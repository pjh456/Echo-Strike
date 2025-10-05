#include <iostream>

#include <SDL3/SDL.h>

#include <echo_strike/device/window.hpp>

int main(int, char **)
{
    if (!SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    try
    {
        // 创建窗口，初始隐藏
        auto window =
            Window::create()
                .set_title("Echo Strike - Move Window Test")
                .size(640, 480)
                .resizable(true)
                .visible(false)
                .build();

        SDL_Delay(500);
        window.show();

        // 设置初始位置
        window.set_position(100, 100);

        bool running = true;
        SDL_Event e;
        int speed = 100; // 每帧移动像素数

        int step1_delay = 100;
        int step2_delay = 100;

        // 记录方向键状态
        bool up = false, down = false, left = false, right = false;

        while (running)
        {
            while (SDL_PollEvent(&e))
            {
                if (e.type == SDL_EVENT_QUIT)
                {
                    running = false;
                }
                else if (e.type == SDL_EVENT_KEY_DOWN)
                {
                    switch (e.key.key)
                    {
                    case SDLK_UP:
                        up = true;
                        break;
                    case SDLK_DOWN:
                        down = true;
                        break;
                    case SDLK_LEFT:
                        left = true;
                        break;
                    case SDLK_RIGHT:
                        right = true;
                        break;
                    }
                }
                else if (e.type == SDL_EVENT_KEY_UP)
                {
                    switch (e.key.key)
                    {
                    case SDLK_UP:
                        up = false;
                        break;
                    case SDLK_DOWN:
                        down = false;
                        break;
                    case SDLK_LEFT:
                        left = false;
                        break;
                    case SDLK_RIGHT:
                        right = false;
                        break;
                    }
                }
            }

            // 获取当前位置
            auto [x, y] = window.get_position();
            auto [w, h] = window.get_size();

            // 根据方向键调整位置
            if (up)
            {
                window.resize(w, h + speed);
                window.set_position(x, y - speed);
                SDL_Delay(step1_delay);
                window.resize(w, h);
            }
            if (down)
            {
                window.resize(w, h + speed);
                SDL_Delay(step1_delay);
                window.resize(w, h);
                window.set_position(x, y + speed);
            }
            if (left)
            {
                window.resize(w + speed, h);
                window.set_position(x - speed, y);
                SDL_Delay(step1_delay);
                window.resize(w, h);
            }
            if (right)
            {
                window.resize(w + speed, h);
                SDL_Delay(step1_delay);
                window.resize(w, h);
                window.set_position(x + speed, y);
            }

            SDL_Delay(step2_delay);
        }
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Exception: " << ex.what() << std::endl;
    }

    SDL_Quit();
    return 0;
}
