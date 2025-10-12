#ifndef INCLUDE_OBJECT
#define INCLUDE_OBJECT

#include <echo_strike/utils/class_marcos.hpp>

#include <echo_strike/utils/vec2.hpp>
#include <echo_strike/transform/rect.hpp>

#include <echo_strike/physics/movement_controller.hpp>

#include <SDL3/SDL.h>

class Object
{
public:
    enum class MotionType
    {
        IgnoreBoundary,
        LimitedInBoundary
    };

protected:
    MotionType m_type = MotionType::IgnoreBoundary;
    Vec2 move_dir;
    MovementController move_ctrl;

    CLASS_PROPERTY(Rect, rect)
    CLASS_PROPERTY(Vec2, speed)
    CLASS_PROPERTY(Vec2, force)
    CLASS_PROPERTY(Rect, boundary)

public:
    Object() = default;
    virtual ~Object() = default;

public:
    virtual void add_speed(const Vec2 &speed) { m_speed += speed; }
    virtual void add_force(const Vec2 &force) { m_force += force; }

public:
    virtual void render_border(SDL_Renderer *renderer) const { m_rect.render_border(renderer); }
    virtual void render_full(SDL_Renderer *renderer) const { m_rect.render_full(renderer); }

    void on_update(float ms)
    {
        float delta = ms / 1000;
        float delta_pos_x = delta * m_speed.get_x();
        float delta_pos_y = delta * m_speed.get_y();
        m_rect.set_x(m_rect.get_x() + delta_pos_x);
        m_rect.set_y(m_rect.get_y() + delta_pos_y);

        Vec2 cur_speed = get_speed();
        Vec2 new_speed = move_ctrl.update_velocity(cur_speed, move_dir, ms);
        set_speed(new_speed);

        float delta_speed_x = delta * m_force.get_x();
        float delta_speed_y = delta * m_force.get_y();
        m_speed.set_x(m_speed.get_x() + delta_speed_x);
        m_speed.set_y(m_speed.get_y() + delta_speed_y);

        if (m_type == MotionType::LimitedInBoundary)
        {
            auto pos = m_rect.get_position();

            if (m_rect.left() < m_boundary.left())
                pos.set_x(m_boundary.left()), m_speed.set_x(0);
            if (m_rect.right() > m_boundary.right())
                pos.set_x(m_boundary.right() - m_rect.get_width()), m_speed.set_x(0);
            if (m_rect.bottom() < m_boundary.bottom())
                pos.set_y(m_boundary.bottom()), m_speed.set_y(0);
            if (m_rect.top() > m_boundary.top())
                pos.set_y(m_boundary.top() - m_rect.get_height()), m_speed.set_y(0);

            m_rect.set_position(pos);
        }
    }

public:
    MotionType get_motion_type() const { return m_type; }
    void set_motion_type(MotionType t) { m_type = t; }

    MovementController &get_movement_controller() { return move_ctrl; }
    const MovementController &get_movement_controller() const { return move_ctrl; }

    Vec2 get_move_direction() const { return move_dir; }
    void set_move_direction(const Vec2 &dir) { move_dir = dir; }
};

#endif // INCLUDE_OBJECT