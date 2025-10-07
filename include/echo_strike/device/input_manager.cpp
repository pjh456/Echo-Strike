#include <echo_strike/device/input_manager.hpp>

#include <SDL3/SDL_events.h>

InputManager &InputManager::instance()
{
    static InputManager manager;
    return manager;
}

void InputManager::update()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_EVENT_KEY_DOWN:
            key_states[e.key.scancode] = true;
            break;
        case SDL_EVENT_KEY_UP:
            key_states[e.key.scancode] = false;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            mouse_states[e.button.button] = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouse_states[e.button.button] = false;
            break;
        case SDL_EVENT_MOUSE_MOTION:
            mouseX = e.motion.x;
            mouseY = e.motion.y;
            break;
        default:
            break;
        }
    }
}