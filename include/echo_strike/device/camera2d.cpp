#include <echo_strike/device/camera2d.hpp>

#include <cmath>

Camera2D::Camera2D(const Rect &rect)
    : m_viewport(rect)
{
}

Point Camera2D::world_to_screen(const Point &world) const
{
    Vec2 rel = world - m_viewport.get_position();

    float cos_r = std::cos(-rotation);
    float sin_r = std::sin(-rotation);

    Vec2 rotated(
        rel.get_x() * cos_r - rel.get_y() * sin_r,
        rel.get_x() * sin_r + rel.get_y() * cos_r);

    return m_origin.get_position() + rotated * zoom;
}

Point Camera2D::screen_to_world(const Point &screen) const
{
    Vec2 rel = (screen - m_origin) / zoom;

    float cos_r = std::cos(rotation);
    float sin_r = std::sin(rotation);

    // 正向旋转
    Vec2 rotated(
        rel.get_x() * cos_r - rel.get_y() * sin_r,
        rel.get_x() * sin_r + rel.get_y() * cos_r);

    return rotated + m_viewport.get_position().get_position();
}

Rect Camera2D::world_to_screen(const Rect &world) const
{
    Point pos = world_to_screen(world.get_position());
    Vec2 size = world.get_size() * zoom;
    return Rect(pos, size);
}

Rect Camera2D::screen_to_world(const Rect &screen) const
{
    Point pos = screen_to_world(screen.get_position());
    Vec2 size = screen.get_size() / zoom;
    return Rect(pos, size);
}