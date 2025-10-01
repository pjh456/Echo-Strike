#include <echo_strike/utils/vec2.hpp>

#include <cmath>

Vec2::Vec2() : m_x(0), m_y(0) {}

Vec2::Vec2(int p_x, int p_y) : m_x(p_x), m_y(p_y) {}

Vec2 Vec2::operator+(const Vec2 &other) const
{
    return Vec2(m_x + other.m_x, m_y + other.m_y);
}

Vec2 Vec2::operator-(const Vec2 &other) const
{
    return Vec2(m_x - other.m_x, m_y - other.m_y);
}

Vec2 Vec2::operator*(const Vec2 &other) const
{
    return Vec2(m_x * other.m_x, m_y * other.m_y);
}

Vec2 Vec2::operator*(int ratio) const
{
    return Vec2(this->m_x * ratio, this->m_y * ratio);
}

Vec2 Vec2::operator/(int ratio) const
{
    if (ratio == 0)
        return Vec2(0, 0);
    else
        return Vec2(this->m_x / ratio, this->m_y / ratio);
}

Vec2 &Vec2::operator+=(const Vec2 &other)
{
    this->m_x += other.m_x;
    this->m_y += other.m_y;
    return *this;
}

Vec2 &Vec2::operator-=(const Vec2 &other)
{
    this->m_x -= other.m_x;
    this->m_y -= other.m_y;
    return *this;
}

Vec2 &Vec2::operator*=(const Vec2 &other)
{
    this->m_x *= other.m_x;
    this->m_y *= other.m_y;
    return *this;
}

Vec2 &Vec2::operator*=(int ratio)
{
    this->m_x *= ratio;
    this->m_y *= ratio;
    return *this;
}

Vec2 &Vec2::operator/=(int ratio)
{
    if (ratio == 0)
        this->m_x = this->m_y = 0;
    else
    {
        this->m_x /= ratio;
        this->m_y /= ratio;
    }

    return *this;
}

float Vec2::length() const
{
    return std::sqrt((m_x * m_x) + (m_y * m_y));
}

Vec2 Vec2::normalize() const
{
    auto len = this->length();
    if (len == 0)
        return Vec2(0, 0);
    return Vec2(m_x / len, m_y / len);
}

float Vec2::euclidean_dis(const Vec2 &other) const
{
    int delta_x = m_x - other.m_x;
    int delta_y = m_y - other.m_y;
    return std::sqrt((delta_x * delta_x) + (delta_y * delta_y));
}

int Vec2::manhattan_dis(const Vec2 &other) const
{
    int delta_x = m_x - other.m_x;
    int delta_y = m_y - other.m_y;
    return std::abs(delta_x) + std::abs(delta_y);
}