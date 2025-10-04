#ifndef INCLUDE_ENTITY
#define INCLUDE_ENTITY

#include <echo_strike/utils/class_marcos.hpp>

#include <echo_strike/physics/object.hpp>

#include <echo_strike/entity/status.hpp>
#include <echo_strike/entity/entity_state.hpp>

#include <echo_strike/state_machine/state_machine.hpp>

#include <functional>

class CollisionBox;

class EntityManager;

class Entity : public Object
{
    friend class EntityManager;

protected:
    Status stus;
    StateMachine anim_sm;

    CLASS_POINTER(CollisionBox, hit_box)
    CLASS_POINTER(CollisionBox, hurt_box)

    CLASS_PROPERTY(Vec2, position)

protected:
    Entity() {}

public:
    virtual ~Entity() = default;

    Entity(const Entity &) = delete;
    Entity &operator=(const Entity &) = delete;

    Entity(Entity &&) noexcept = default;
    Entity &operator=(Entity &&) noexcept = default;

public:
    virtual void on_update(float delta)
    {
        anim_sm.on_update(delta);
    }

    virtual void on_hurt(Entity *) {}

public:
    virtual Status &get_status() { return stus; }
    virtual const Status &get_status() const { return stus; }

    virtual StateMachine &get_state_machine() { return anim_sm; }
    virtual const StateMachine &get_state_machine() const { return anim_sm; }
};

#endif // INCLUDE_ENTITY