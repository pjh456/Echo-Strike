#ifndef INCLUDE_EVENTS
#define INCLUDE_EVENTS

#include <echo_strike/event/event.hpp>
#include <echo_strike/physics/object.hpp>
#include <echo_strike/utils/vec2.hpp>

#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_mouse.h>

#include <tuple>

class KeyPressedEvent : public Event
{
private:
    SDL_Scancode key;

public:
    KeyPressedEvent(SDL_Scancode k)
        : Event(EventType::KeyPressed),
          key(k) {}

    ~KeyPressedEvent() override = default;

public:
    SDL_Scancode get_key() const { return key; }
};

class KeyReleasedEvent : public Event
{
private:
    SDL_Scancode key;

public:
    KeyReleasedEvent(SDL_Scancode k)
        : Event(EventType::KeyPressed),
          key(k) {}
    ~KeyReleasedEvent() override = default;

public:
    SDL_Scancode get_key() const { return key; }
};

class KeyQueryEvent : public Event
{
private:
    SDL_Scancode key;
    bool &result;

public:
    KeyQueryEvent(SDL_Scancode k, bool &res)
        : Event(EventType::KeyQuery),
          key(k),
          result(res) {}
    ~KeyQueryEvent() override = default;

public:
    SDL_Scancode get_key() const { return key; }
    void set_result(bool res) { result = res; }
};

class MousePressedEvent : public Event
{
private:
    Uint8 button;

public:
    MousePressedEvent(Uint8 b)
        : Event(EventType::MousePressed),
          button(b) {}
    ~MousePressedEvent() override = default;

public:
    Uint8 get_button() const { return button; }
};

class MouseReleasedEvent : public Event
{
private:
    Uint8 button;

public:
    MouseReleasedEvent(Uint8 b)
        : Event(EventType::MouseReleased),
          button(b) {}
    ~MouseReleasedEvent() override = default;

public:
    Uint8 get_button() const { return button; }
};

class MouseMoveEvent : public Event
{
private:
    Vec2 world_pos;
    Vec2 delta_pos;

public:
    MouseMoveEvent(
        const Vec2 &world,
        const Vec2 &delta)
        : Event(EventType::MouseMove),
          world_pos(world),
          delta_pos(delta) {}
    ~MouseMoveEvent() override = default;

public:
    Vec2 get_world_pos() const { return world_pos; }
    Vec2 get_delta_pos() const { return delta_pos; }
};

class CollisionEvent : public Event
{
private:
    Object *obj1;
    Object *obj2;

public:
    CollisionEvent(Object *a, Object *b)
        : Event(EventType::Collision),
          obj1(a), obj2(b) {}
    ~CollisionEvent() override = default;

public:
    std::tuple<Object *, Object *> get_objects() const { return {obj1, obj2}; }
};

#endif // INCLUDE_EVENTS