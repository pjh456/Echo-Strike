#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL 初始化失败: %s\n", SDL_GetError());
        return -1;
    }

    auto window = SDL_CreateWindow(
        "SDL3 Demo",
        800, 600, // 宽高
        SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        printf("窗口创建失败: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    auto renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer)
    {
        printf("渲染器创建失败: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    int running = 1;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            { // 点击关闭按钮
                running = 0;
            }
        }

        // 设置背景色（R,G,B,A）
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        SDL_RenderClear(renderer);

        // 画一个矩形
        SDL_FRect rect = {200, 150, 400, 300};
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        SDL_RenderFillRect(renderer, &rect);

        // 交换缓冲区
        SDL_RenderPresent(renderer);
    }

    // 清理资源
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}