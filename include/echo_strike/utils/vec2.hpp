#ifndef INCLUDE_VEC2
#define INCLUDE_VEC2

#include <echo_strike/utils/class_marcos.hpp>

#include <ostream>

class Vec2
{
    CLASS_PROPERTY(float, x);
    CLASS_PROPERTY(float, y);

public:
    Vec2();
    Vec2(float, float);

    DEFAULT_CONSTRUCTOR(Vec2);

    DEFAULT_EQUAL_OPERATOR(Vec2);

public:
    Vec2 operator+(const Vec2 &) const;
    Vec2 operator-(const Vec2 &) const;
    Vec2 operator*(const Vec2 &) const;

    Vec2 operator*(float) const;
    Vec2 operator/(float) const;

    Vec2 &operator+=(const Vec2 &);
    Vec2 &operator-=(const Vec2 &);
    Vec2 &operator*=(const Vec2 &);

    Vec2 &operator*=(float);
    Vec2 &operator/=(float);

public:
    float length() const;
    Vec2 normalize() const;

    float euclidean_dis(const Vec2 &) const;
    float manhattan_dis(const Vec2 &) const;
};

inline std::ostream &operator<<(std::ostream &os, const Vec2 &vec2)
{
    os << "Vec2(" << vec2.get_x() << ", " << vec2.get_y() << ")";
    return os;
}

#endif // INCLUDE_VEC2