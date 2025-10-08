#ifndef INCLUDE_CAMERA_2D
#define INCLUDE_CAMERA_2D

#include <echo_strike/utils/vec2.hpp>
#include <echo_strike/transform/point.hpp>
#include <echo_strike/transform/rect.hpp>

class Camera2D
{
private:
    Rect m_viewport;
    Point m_origin;
    float zoom = 1.0f;
    float rotation = 0.0f; // No rotation!

public:
    Camera2D(const Rect &);

public:
    Point world_to_screen(const Point &point) const;
    Point screen_to_world(const Point &point) const;

    Rect world_to_screen(const Rect &point) const;
    Rect screen_to_world(const Rect &point) const;

public:
    Rect get_viewport() const { return m_viewport; }
    void set_viewport(const Rect &rect) { m_viewport = rect; }
    void move(const Vec2 &delta) { m_viewport += delta; }

    float get_zoom() const { return zoom; }
    void set_zoom(float factor) { zoom = factor; }
    void zoom_by(float factor)
    {
        if (factor != 0.0f)
            zoom *= factor;
    }

    // float get_rotation() const { return rotation; }
    // void set_rotation(float radians) { rotation = radians; }
    // void rotate_by(float radians) { rotation += radians; }

    Point get_origin() const { return m_origin; }
    void set_origin(const Point &point) { m_origin = point; }
};

#endif // INCLUDE_CAMERA_2D