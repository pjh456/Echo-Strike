#ifndef INCLUDE_RECT
#define INCLUDE_RECT

#include <echo_strike/utils/class_marcos.hpp>

#include <echo_strike/utils/vec2.hpp>
#include <echo_strike/utils/ray.hpp>

#include <SDL3/SDL.h>

#include <initializer_list>
#include <ostream>

class Rect
{
private:
    CLASS_PROPERTY(float, x);
    CLASS_PROPERTY(float, y);
    CLASS_PROPERTY(float, width);
    CLASS_PROPERTY(float, height);

public:
    Rect(float, float, float, float);
    Rect();
    Rect(const SDL_Rect &);
    Rect(const SDL_FRect &);

    DEFAULT_CONSTRUCTOR(Rect);

    DEFAULT_EQUAL_OPERATOR(Rect);

public:
    Vec2 position() const;
    Vec2 size() const;

    SDL_Rect to_rect() const;
    SDL_FRect to_frect() const;

public:
    float left() const { return m_x; }
    float right() const { return m_x + m_width; }
    float top() const { return m_y + m_height; }
    float bottom() const { return m_y; }

public:
    Vec2 top_left() const { return Vec2(left(), top()); }
    Vec2 top_right() const { return Vec2(right(), top()); }
    Vec2 bottom_left() const { return Vec2(left(), bottom()); }
    Vec2 bottom_right() const { return Vec2(right(), bottom()); }

public:
    static Rect bounding_box(std::initializer_list<Rect>);

public:
    bool is_strictly_inside(const Vec2 &) const;
    bool is_strictly_inside(const Rect &) const;

    bool is_inside(const Vec2 &) const;
    bool is_inside(const Rect &) const;

    bool is_on_edge(const Vec2 &) const;
    bool is_on_edge(const Rect &) const;

    bool is_intersect(const Vec2 &) const;
    bool is_intersect(const Ray &) const;
    bool is_intersect(const Rect &) const;

    void render_border(SDL_Renderer *) const;
    void render_full(SDL_Renderer *) const;

public:
    Rect &operator+=(const Vec2 &);
    Rect operator+(const Vec2 &) const;

    Rect &operator-=(const Vec2 &);
    Rect operator-(const Vec2 &) const;

    Rect &operator*=(const Vec2 &);
    Rect operator*(const Vec2 &) const;

    Rect &operator/=(const Vec2 &);
    Rect operator/(const Vec2 &) const;

public:
    float time_to_collide(const Vec2 &, const Rect &) const;
};

inline std::ostream &operator<<(std::ostream &os, const Rect &rect)
{
    os << "Rect(" << rect.position() << ", " << rect.position() + rect.size() << ")";
    return os;
}

#endif // INCLUDE_RECT