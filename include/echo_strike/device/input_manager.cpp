#include <echo_strike/device/input_manager.hpp>

#include <SDL3/SDL_events.h>

#include <echo_strike/event/events.hpp>
#include <echo_strike/event/event_bus.hpp>

#include <memory>

InputManager::InputManager()
{
    EventBus::instance()
        .subscribe<KeyQueryEvent>(
            [this](KeyQueryEvent *event)
            {
                event->set_result(key_states[event->get_key()]);
            });
}

InputManager &InputManager::instance()
{
    static InputManager manager;
    return manager;
}

void InputManager::update()
{
    SDL_Event e;
    auto &bus = EventBus::instance();
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_EVENT_KEY_DOWN:
            bus.publish(std::make_shared<KeyPressedEvent>(e.key.scancode));
            key_states[e.key.scancode] = true;
            break;
        case SDL_EVENT_KEY_UP:
            bus.publish(std::make_shared<KeyReleasedEvent>(e.key.scancode));
            key_states[e.key.scancode] = false;
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            bus.publish(std::make_shared<MousePressedEvent>(e.button.button));
            mouse_states[e.button.button] = true;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            bus.publish(std::make_shared<MouseReleasedEvent>(e.button.button));
            mouse_states[e.button.button] = false;
            break;
        case SDL_EVENT_MOUSE_MOTION:
        {
            Vec2 cur = {e.motion.x, e.motion.y};
            auto dt = cur - mouse_pos;
            bus.publish(std::make_shared<MouseMoveEvent>(cur, dt));
            mouse_pos = cur;
            break;
        }
        default:
            break;
        }
    }
}