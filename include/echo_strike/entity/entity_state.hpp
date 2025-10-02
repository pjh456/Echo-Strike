#ifndef INCLUDE_ENTITY_STATE
#define INCLUDE_ENTITY_STATE

#include <echo_strike/state_machine/state_node.hpp>
#include <echo_strike/state_machine/state_machine.hpp>

#include <echo_strike/image/animation.hpp>

#include <string>

class EntityState : public StateNode
{
private:
    Animation anim;

public:
    EntityState() : anim(50) {}
    ~EntityState() = default;

public:
    void on_enter() override {}
    void on_update(float delta) override { anim.update(delta); }
    void on_exit() override {}

public:
    Animation &get_anim() { return anim; }
    const Animation &get_anim() const { return anim; }
};

#endif