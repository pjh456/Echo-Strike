#ifndef INCLUDE_PARTICLE
#define INCLUDE_PARTICLE

#include <echo_strike/utils/class_marcos.hpp>
#include <echo_strike/utils/vec2.hpp>
#include <echo_strike/utils/rect.hpp>

#include <SDL3/SDL.h>

class Particle
{
private:
    CLASS_PROPERTY(Rect, rect)
    CLASS_PROPERTY(Vec2, speed)
    CLASS_PROPERTY(Vec2, force)

public:
    Particle() = default;
    ~Particle() = default;

public:
    void add_speed(const Vec2 &speed) { m_speed += speed; }
    void add_force(const Vec2 &force) { m_force += force; }

public:
    void render_border(SDL_Renderer *renderer) const { m_rect.render_border(renderer); }
    void render_full(SDL_Renderer *renderer) const { m_rect.render_full(renderer); }

    void on_update(float delta)
    {
        float delta_pos_x = delta * m_speed.get_x();
        float delta_pos_y = delta * m_speed.get_y();
        m_rect.set_x(m_rect.get_x() + delta_pos_x);
        m_rect.set_y(m_rect.get_y() + delta_pos_y);

        float delta_speed_x = delta * m_force.get_x();
        float delta_speed_y = delta * m_force.get_y();
        m_speed.set_x(m_speed.get_x() + delta_speed_x);
        m_speed.set_y(m_speed.get_y() + delta_speed_y);
    }
};

#endif // INCLUDE_PARTICLE