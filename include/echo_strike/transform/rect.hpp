#ifndef INCLUDE_RECT
#define INCLUDE_RECT

#include <echo_strike/utils/class_marcos.hpp>

#include <echo_strike/utils/vec2.hpp>
#include <echo_strike/utils/color.hpp>
#include <echo_strike/transform/point.hpp>

#include <SDL3/SDL.h>

#include <initializer_list>
#include <ostream>
#include <algorithm>

class Rect
{
private:
    Point pos;
    Vec2 size;

public:
    Rect(const Point &p, const Vec2 &v) : pos(p), size(v) {}
    Rect(float x, float y, float w, float h) : Rect(Point(x, y), Vec2(w, h)) {}
    Rect() : Rect(Point(), Vec2()) {}
    Rect(const SDL_Rect &rect) : Rect(rect.x, rect.y, rect.w, rect.h) {}
    Rect(const SDL_FRect &rect) : Rect(rect.x, rect.y, rect.w, rect.h) {}

    Rect(const Rect &) = default;
    Rect &operator=(const Rect &) = default;

    Rect(Rect &&) noexcept = default;
    Rect &operator=(Rect &&) = default;

public:
    bool operator==(const Rect &other) const = default;

    void move(const Vec2 &delta) { pos.move(delta); }

public:
    Point get_position() const { return pos; }
    void set_position(const Point &p) { pos = p; }

    Vec2 get_size() const { return size; }
    void set_size(const Vec2 &s) { size = s; }

    SDL_Rect to_rect() const { return {(int)pos.get_x(), (int)pos.get_y(), (int)size.get_x(), (int)size.get_y()}; }
    SDL_FRect to_frect() const { return {pos.get_x(), pos.get_y(), size.get_x(), size.get_y()}; }

public:
    float get_x() const { return pos.get_x(); }
    void set_x(float x) { pos.set_x(x); }

    float get_y() const { return pos.get_y(); }
    void set_y(float y) { pos.set_y(y); }

    float get_width() const { return size.get_x(); }
    void set_width(float w) { size.set_x(w); }

    float get_height() const { return size.get_y(); }
    void set_height(float h) { size.set_y(h); }

public:
    float left() const { return pos.get_x(); }
    float right() const { return pos.get_x() + size.get_x(); }
    float top() const { return pos.get_y() + size.get_y(); }
    float bottom() const { return pos.get_y(); }

public:
    Vec2 top_left() const { return Vec2(left(), top()); }
    Vec2 top_right() const { return Vec2(right(), top()); }
    Vec2 bottom_left() const { return Vec2(left(), bottom()); }
    Vec2 bottom_right() const { return Vec2(right(), bottom()); }
    Vec2 center() const { return (top_left() + bottom_right()) / 2.0f; }

public:
    static Rect bounding_box(std::initializer_list<Rect> rects)
    {
        auto min_x =
            std::min_element(
                rects.begin(),
                rects.end(),
                [](auto const &a, auto const &b)
                { return a.left() < b.left(); })
                ->left();

        auto min_y =
            std::min_element(
                rects.begin(),
                rects.end(),
                [](auto const &a, auto const &b)
                { return a.bottom() < b.bottom(); })
                ->bottom();

        auto max_x =
            std::max_element(
                rects.begin(),
                rects.end(),
                [](auto const &a, auto const &b)
                { return a.right() < b.right(); })
                ->right();

        auto max_y =
            std::max_element(
                rects.begin(),
                rects.end(),
                [](auto const &a, auto const &b)
                { return a.top() < b.top(); })
                ->top();

        return Rect(min_x, min_y, max_x - min_x, max_y - min_y);
    }

public:
    bool is_strictly_inside(const Point &point) const
    {
        if (left() > point.get_x())
            return false;
        if (right() < point.get_x())
            return false;
        if (top() < point.get_y())
            return false;
        if (bottom() > point.get_y())
            return false;
        return true;
    }
    bool is_strictly_inside(const Rect &rect) const
    {
        if (left() <= rect.left())
            return false;
        if (right() >= rect.right())
            return false;
        if (top() >= rect.top())
            return false;
        if (bottom() <= rect.bottom())
            return false;
        return true;
    }

    bool is_inside(const Point &point) const
    {
        return is_strictly_inside(point) || is_on_edge(point);
    }
    bool is_inside(const Rect &rect) const
    {
        if (left() < rect.left())
            return false;
        if (right() > rect.right())
            return false;
        if (top() > rect.top())
            return false;
        if (bottom() < rect.bottom())
            return false;
        return true;
    }

    bool is_on_edge(const Point &point) const
    {
        if (left() == point.get_x())
            return true;
        if (right() == point.get_x())
            return true;
        if (top() == point.get_y())
            return true;
        if (bottom() == point.get_y())
            return true;
        return false;
    }
    bool is_on_edge(const Rect &rect) const
    {
        if (left() == rect.left() || left() == rect.right())
            return true;
        if (right() == rect.right() || right() == rect.left())
            return true;
        if (top() == rect.top() || top() == rect.bottom())
            return true;
        if (bottom() == rect.bottom() || bottom() == rect.top())
            return true;
        return false;
    }

    bool is_intersect(const Point &point) const
    {
        return is_inside(point) || is_on_edge(point);
    }
    bool is_intersect(const Rect &rect) const
    {
        if (right() < rect.left() || left() > rect.right())
            return false;
        if (bottom() > rect.top() || top() < rect.bottom())
            return false;

        return true;
    }

    void render_border(SDL_Renderer *renderer) const
    {
        auto rect = to_frect();
        static Color color(Color::random_color());

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
        SDL_RenderRect(renderer, &rect);
    }
    void render_full(SDL_Renderer *renderer) const
    {
        auto rect = to_frect();
        static Color color(Color::random_color());

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
        SDL_RenderFillRect(renderer, &rect);
    }

public:
    Rect &operator+=(const Vec2 &delta) { return pos += delta, *this; }
    Rect operator+(const Vec2 &delta) const { return Rect(*this).operator+=(delta); }

    Rect &operator-=(const Vec2 &delta) { return pos -= delta, *this; }
    Rect operator-(const Vec2 &delta) const { return Rect(*this).operator-=(delta); }

    Rect &operator*=(const Vec2 &ratio) { return size * ratio, *this; }
    Rect operator*(const Vec2 &ratio) const { return Rect(*this).operator*=(ratio); }

    Rect &operator/=(const Vec2 &ratio) { return size / ratio, *this; }
    Rect operator/(const Vec2 &ratio) const { return Rect(*this).operator*=(ratio); }

public:
    float time_to_collide(const Vec2 &velocity, const Rect &target) const
    {
        Vec2 enter_time, exit_time;

        float vx = velocity.get_x();
        float vy = velocity.get_y();

        if (vx > 0)
        {
            enter_time.set_x((target.left() - right()) / vx);
            exit_time.set_x((target.right() - left()) / vx);
        }
        else if (vx < 0)
        {
            enter_time.set_x((target.right() - left()) / vx);
            exit_time.set_x((target.left() - right()) / vx);
        }
        else
        {
            if (right() < target.left() || left() > target.right())
                return -1;
            else
            {
                enter_time.set_x(-std::numeric_limits<float>::infinity());
                exit_time.set_x(+std::numeric_limits<float>::infinity());
            }
        }

        if (vy > 0)
        {
            enter_time.set_y((target.bottom() - top()) / vy);
            exit_time.set_y((target.top() - bottom()) / vy);
        }
        else if (vy < 0)
        {
            enter_time.set_y((target.top() - bottom()) / vy);
            exit_time.set_y((target.bottom() - top()) / vy);
        }
        else
        {
            if (top() < target.bottom() || bottom() > target.top())
                return -1;
            else
            {
                enter_time.set_y(-std::numeric_limits<float>::infinity());
                exit_time.set_y(std::numeric_limits<float>::infinity());
            }
        }

        float t_enter = std::max(enter_time.get_x(), enter_time.get_y());
        float t_exit = std::min(exit_time.get_x(), exit_time.get_y());

        if (t_enter > t_exit || t_exit < 0)
            return -1.0f; // 不会碰撞

        return std::max(0.0f, t_enter); // 碰撞时间
    }
};

inline std::ostream &operator<<(std::ostream &os, const Rect &rect)
{
    os << "Rect(" << rect.top_left() << ", " << rect.bottom_right() << ")";
    return os;
}

#endif // INCLUDE_RECT