#ifndef INCLUDE_BLEND_MODE
#define INCLUDE_BLEND_MODE

#include <cstdint>
#include <SDL3/SDL_blendmode.h>

enum class BlendMode : uint32_t
{
    None = SDL_BLENDMODE_NONE,
    Blend = SDL_BLENDMODE_BLEND,
    BlendPremutiplied = SDL_BLENDMODE_BLEND_PREMULTIPLIED,
    Add = SDL_BLENDMODE_ADD,
    AddPremutiplied = SDL_BLENDMODE_ADD_PREMULTIPLIED,
    Mod = SDL_BLENDMODE_MOD,
    Mul = SDL_BLENDMODE_MUL,
    Invalid = SDL_BLENDMODE_INVALID
};

#endif // INCLUDE_BLEND_MODE