#ifndef INCLUDE_INPUT_EVENT
#define INCLUDE_INPUT_EVENT

#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_keycode.h>

enum class InputType
{
    Keyboard,
    Mouse,
    Controller
};

enum class KeyState
{
    Pressed,
    Released,
    Held
};

struct InputEvent
{
    InputType type;
    SDL_Scancode key;
    Uint8 mouseButton;
    KeyState state;
    int x = 0, y = 0;
};

#endif // INCLUDE_INPUT_EVENT