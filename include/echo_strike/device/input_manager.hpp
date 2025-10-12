#ifndef INCLUDE_INPUT_MANAGER
#define INCLUDE_INPUT_MANAGER

#include <echo_strike/utils/vec2.hpp>

#include <unordered_map>
#include <SDL3/SDL_scancode.h>

class InputManager
{
public:
    static InputManager &instance();

private:
    std::unordered_map<SDL_Scancode, bool> key_states;
    std::unordered_map<Uint8, bool> mouse_states;
    Vec2 mouse_pos;

private:
    InputManager();
    ~InputManager() = default;

    InputManager(const InputManager &) = delete;
    InputManager &operator=(const InputManager &) = delete;

    InputManager(InputManager &&) noexcept = delete;
    InputManager &operator=(InputManager &) noexcept = delete;

public:
    void update();

    bool is_key_pressed(SDL_Scancode scancode) const
    {
        auto it = key_states.find(scancode);
        return it != key_states.end() && it->second;
    }

    bool is_mouse_button_pressed(Uint8 button) const
    {
        auto it = mouse_states.find(button);
        return it != mouse_states.end() && it->second;
    }

    Vec2 get_mouse_position() const { return mouse_pos; }
};

#endif // INCLUDE_INPUT_MANAGER