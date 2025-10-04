#ifndef INCLUDE_RAY
#define INCLUDE_RAY

#include <echo_strike/utils/class_marcos.hpp>

#include <echo_strike/utils/vec2.hpp>

class Ray
{
private:
    CLASS_PROPERTY(Vec2, src);
    CLASS_PROPERTY(Vec2, dst);

public:
    Ray() = default;
    Ray(const Vec2 &p_src, const Vec2 &p_dst)
        : m_src(p_src),
          m_dst(p_dst)
    {
    }
    DEFAULT_CONSTRUCTOR(Ray)

    DEFAULT_EQUAL_OPERATOR(Ray)

public:
    float length() const;
    Ray normalize() const;

    bool is_on(const Vec2 &) const;
    bool is_intersect(const Ray &) const;

public:
    Ray &operator+=(const Ray &);
    Ray operator+(const Ray &) const;

    Ray &operator-=(const Ray &);
    Ray operator-(const Ray &) const;

    Ray &operator*=(float);
    Ray operator*(float) const;

    Ray &operator/=(float);
    Ray operator/(float) const;
};

inline Ray operator-(const Ray &ray)
{
    return Ray().operator-(ray);
}

#endif // INCLUDE_RAY