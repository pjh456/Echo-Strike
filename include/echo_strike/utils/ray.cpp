#include <echo_strike/utils/ray.hpp>

float Ray::length() const { return (m_dst - m_src).length(); }

Ray Ray::normalize() const { return Ray(m_src, m_src + (m_dst - m_src).normalize()); }

bool Ray::is_on(const Vec2 &pos) const
{
    if (std::abs((m_dst - m_src).cross(pos - m_src)) > 1e-6)
        return false;

    bool within_x =
        pos.get_x() >= std::min(m_src.get_x(), m_dst.get_x()) - 1e-6 &&
        pos.get_x() <= std::max(m_src.get_x(), m_dst.get_x()) + 1e-6;

    bool within_y =
        pos.get_y() >= std::min(m_src.get_y(), m_dst.get_y()) - 1e-6 &&
        pos.get_y() <= std::max(m_src.get_y(), m_dst.get_y()) + 1e-6;

    return within_x && within_y;
}

bool Ray::is_intersect(const Ray &other) const
{
    Vec2 r = m_dst - m_src;
    Vec2 s = other.m_dst - other.m_src;
    Vec2 pq = other.m_src - m_src;

    float rxs = r.cross(s);
    float pqxr = pq.cross(r);

    if (rxs == 0)
    {
        if (pqxr != 0)
            return false;

        return is_on(other.m_src) || is_on(other.m_dst) ||
               other.is_on(m_src) || other.is_on(m_dst);
    }
    else
    {
        float t = pq.cross(s) / rxs;
        float u = pqxr / rxs;
        return t >= 0 && t <= 1 && u >= 0 && u <= 1;
    }
}

Ray &Ray::operator+=(const Ray &other)
{
    m_dst += other.m_dst - other.m_src;
    return *this;
}
Ray Ray::operator+(const Ray &other) const
{
    return Ray(*this).operator+=(other);
}

Ray &Ray::operator-=(const Ray &other)
{
    m_dst += other.m_src - other.m_dst;
    return *this;
}
Ray Ray::operator-(const Ray &other) const
{
    return Ray(*this).operator-=(other);
}

Ray &Ray::operator*=(float ratio)
{
    auto dir = m_dst - m_src;
    m_dst = dir * ratio + m_src;
    return *this;
}
Ray Ray::operator*(float ratio) const
{
    return Ray(*this).operator*=(ratio);
}

Ray &Ray::operator/=(float ratio)
{
    if (ratio == 0)
        return m_dst = m_src, *this;

    auto dir = m_dst - m_src;
    m_dst = dir / ratio + m_src;
    return *this;
}
Ray Ray::operator/(float ratio) const
{
    return Ray(*this).operator/=(ratio);
}