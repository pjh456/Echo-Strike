#ifndef INCLUDE_ENTITY_STATE
#define INCLUDE_ENTITY_STATE

#include <echo_strike/state_machine/state_node.hpp>
#include <echo_strike/state_machine/state_machine.hpp>

#include <echo_strike/image/animation.hpp>

#include <string>

class Object;

class EntityState : public StateNode
{
private:
    Animation anim;
    Object *obj;

public:
    EntityState(Object *o = nullptr) : anim(50), obj(o) {}
    ~EntityState() = default;

public:
    void on_enter() override {}
    void on_update(float delta) override;
    void on_exit() override {}

public:
    Animation &get_anim() { return anim; }
    const Animation &get_anim() const { return anim; }

    Object *get_object() const { return obj; }
    void set_object(Object *o) { obj = o; }
};

#endif