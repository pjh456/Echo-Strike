#ifndef INCLUDE_ENTITY
#define INCLUDE_ENTITY

#include <echo_strike/utils/class_marcos.hpp>

#include <echo_strike/physics/object.hpp>

#include <echo_strike/entity/status.hpp>
#include <echo_strike/entity/entity_state.hpp>

#include <echo_strike/state_machine/state_machine.hpp>

#include <functional>

class CollisionBox;

class Entity : public Object
{
protected:
    const float FLOOR = 640.0f;
    const float WIDTH = 800.0f;

protected:
    Status stus;
    StateMachine anim_sm;

    CLASS_POINTER(CollisionBox, hit_box)
    CLASS_POINTER(CollisionBox, hurt_box)

public:
    Entity();

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
    virtual void on_update(float delta)
    {
        Object::on_update(delta);
        anim_sm.on_update(delta);
        fix_position();
    }

    virtual void on_hurt(int val, Entity * = nullptr) {}

    virtual void on_render(SDL_Renderer *renderer)
    {
        auto raw_ptr = anim_sm.get_current_state();
        if (!raw_ptr)
            return;

        if (auto it = dynamic_cast<EntityState *>(raw_ptr))
        {
            it->get_anim().render(renderer);
        }
    }

public:
    virtual Status &get_status() { return stus; }
    virtual const Status &get_status() const { return stus; }

    virtual StateMachine &get_state_machine() { return anim_sm; }
    virtual const StateMachine &get_state_machine() const { return anim_sm; }

protected:
    virtual void fix_position()
    {
        auto rect = Object::m_rect;
        if (m_rect.bottom() <= 0)
            m_rect.set_y(0);
        if (m_rect.top() >= FLOOR)
            m_rect.set_y(FLOOR - m_rect.get_height());
        if (m_rect.left() <= 0)
            m_rect.set_x(0);
        if (m_rect.right() >= WIDTH)
            m_rect.set_x(WIDTH - m_rect.get_width());
    }
};

#endif // INCLUDE_ENTITY