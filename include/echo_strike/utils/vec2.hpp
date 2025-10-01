#ifndef INCLUDE_VEC2
#define INCLUDE_VEC2

#include <echo_strike/utils/class_marcos.hpp>

#include <ostream>

class Vec2
{
    CLASS_PROPERTY(int, x);
    CLASS_PROPERTY(int, y);

public:
    Vec2();
    Vec2(int, int);

    DEFAULT_CONSTRUCTOR(Vec2);

    DEFAULT_EQUAL_OPERATOR(Vec2);

public:
    Vec2 operator+(const Vec2 &) const;
    Vec2 operator-(const Vec2 &) const;
    Vec2 operator*(const Vec2 &) const;

    Vec2 operator*(int) const;
    Vec2 operator/(int) const;

    Vec2 &operator+=(const Vec2 &);
    Vec2 &operator-=(const Vec2 &);
    Vec2 &operator*=(const Vec2 &);

    Vec2 &operator*=(int);
    Vec2 &operator/=(int);

public:
    float length() const;
    Vec2 normalize() const;

    float euclidean_dis(const Vec2 &) const;
    int manhattan_dis(const Vec2 &) const;
};

inline std::ostream &operator<<(std::ostream &os, const Vec2 &vec2)
{
    os << "Vec2(" << vec2.get_x() << ", " << vec2.get_y() << ")";
    return os;
}

#endif // INCLUDE_VEC2