#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>

int main(int argc, char **argv)
{
    if (!SDL_Init(SDL_INIT_VIDEO))

    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    if (!TTF_Init())
    {
        std::cerr << "TTF_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    std::cout << "SDL3, SDL3_image, SDL3_ttf all initialized successfully!\n";

    // 清理
    TTF_Quit();
    SDL_Quit();
    return 0;
}
