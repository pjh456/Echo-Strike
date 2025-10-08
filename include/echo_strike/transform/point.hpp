#ifndef INCLUDE_POINT
#define INCLUDE_POINT

#include <echo_strike/utils/vec2.hpp>

#include <utility>

class Point
{
private:
    Vec2 m_pos;

public:
    Point() = default;
    ~Point() = default;

    Point(const Point &) = default;
    Point &operator=(const Point &) = default;

    Point(Point &&) noexcept = default;
    Point &operator=(Point &&) noexcept = default;

    Point(const Vec2 &v) : m_pos(v) {}
    Point(float x, float y) : Point(Vec2(x, y)) {}
    Point &operator=(const Vec2 &v) { return m_pos = v, *this; }
    Point &operator=(Vec2 &&v) noexcept { return m_pos = std::move(v), *this; }

public:
    bool operator==(const Point &other) const { return m_pos == other.m_pos; }

public:
    Point &operator+=(const Vec2 &delta) { return m_pos += delta, *this; }
    Point operator+(const Vec2 &delta) const { return Point(m_pos).operator+=(delta); }

    Point &operator-=(const Vec2 &delta) { return m_pos -= delta, *this; }
    Point operator-(const Vec2 &delta) const { return Point(m_pos).operator-=(delta); }

public:
    Vec2 operator-(const Point &other) const { return m_pos - other.m_pos; }

public:
    Vec2 get_position() const { return m_pos; }
    void set_pos(const Vec2 &v) { m_pos = v; }
    void move(const Vec2 &delta) { m_pos += delta; }

    float get_x() const { return m_pos.get_x(); }
    void set_x(float v) { m_pos.set_x(v); }

    float get_y() const { return m_pos.get_y(); }
    void set_y(float v) { m_pos.set_y(v); }
};

inline Point operator+(const Vec2 &v, const Point &p) { return p.operator+(v); }
inline Point operator-(const Vec2 &v, const Point &p) { return p.operator-(v); }

#endif // INCLUDE_POINT