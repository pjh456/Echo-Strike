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

    CollisionBox *m_hit_box;
    CollisionBox *m_hurt_box;

protected:
    Entity();

public:
    virtual ~Entity();

    Entity(const Entity &) = delete;
    Entity &operator=(const Entity &) = delete;

    Entity(Entity &&) noexcept = default;
    Entity &operator=(Entity &&) noexcept = default;

public:
    virtual void decrease_hp(int val, Entity *entity = nullptr)
    {
        stus.set_hp(stus.get_hp() - val);
        on_hurt(val, entity);
    }

public:
    virtual void on_input() {}
    virtual void on_update(float);

    virtual void on_hurt(int, Entity * = nullptr) {}

    virtual void on_render(Renderer *);

    void load_animations(const std::string &key);

public:
    virtual Status &get_status() { return stus; }
    virtual const Status &get_status() const { return stus; }

    virtual StateMachine &get_state_machine() { return anim_sm; }
    virtual const StateMachine &get_state_machine() const { return anim_sm; }

public:
    CollisionBox *get_hit_box() const { return m_hit_box; }
    void set_hit_box(CollisionBox *);

    CollisionBox *get_hurt_box() const { return m_hurt_box; }
    void set_hurt_box(CollisionBox *);
};

#endif // INCLUDE_ENTITY