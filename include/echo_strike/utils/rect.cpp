#include <echo_strike/utils/rect.hpp>

#include <echo_strike/utils/color.hpp>

Rect::Rect(float x, float y, float w, float h)
    : m_x(x), m_y(y),
      m_width(w), m_height(h)
{
}

Rect::Rect() : Rect(0, 0, 0, 0) {}

Rect::Rect(const SDL_Rect &rect)
    : Rect(rect.x, rect.y, rect.w, rect.h) {}

Rect::Rect(const SDL_FRect &rect)
    : Rect((float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h) {}

Vec2 Rect::position() const
{
    return Vec2(m_x, m_y);
}

Vec2 Rect::size() const
{
    return Vec2(m_width, m_height);
}

SDL_Rect Rect::to_rect() const
{
    SDL_Rect ret;
    ret.x = m_x, ret.y = m_y;
    ret.w = m_width, ret.h = m_height;
    return ret;
}

SDL_FRect Rect::to_frect() const
{
    SDL_FRect ret;
    ret.x = m_x, ret.y = m_y;
    ret.w = m_width, ret.h = m_height;
    return ret;
}

bool Rect::is_strictly_inside(const Vec2 &pos) const
{
    if (left() > pos.get_x())
        return false;
    if (right() < pos.get_x())
        return false;
    if (top() < pos.get_y())
        return false;
    if (bottom() > pos.get_y())
        return false;
    return true;
}

bool Rect::is_strictly_inside(const Rect &rect) const
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

bool Rect::is_inside(const Vec2 &pos) const
{
    return is_strictly_inside(pos) || is_on_edge(pos);
}

bool Rect::is_inside(const Rect &rect) const
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

bool Rect::is_on_edge(const Vec2 &pos) const
{
    if (left() == pos.get_x())
        return true;
    if (right() == pos.get_x())
        return true;
    if (top() == pos.get_y())
        return true;
    if (bottom() == pos.get_y())
        return true;
    return false;
}

bool Rect::is_on_edge(const Rect &rect) const
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

bool Rect::is_intersect(const Vec2 &pos) const
{
    return is_inside(pos) || is_on_edge(pos);
}

bool Rect::is_interset(const Ray &ray) const
{

    Ray left_ray(top_left(), bottom_left()),
        right_ray(top_right(), bottom_right()),
        top_ray(top_left(), top_right()),
        bottom_ray(bottom_left(), bottom_right());

    return ray.is_intersect(left_ray) ||
           ray.is_intersect(right_ray) ||
           ray.is_intersect(top_ray) ||
           ray.is_intersect(bottom_ray);
}

bool Rect::is_intersect(const Rect &rect) const
{
    if (right() < rect.left() || left() > rect.right())
        return false;

    if (bottom() > rect.top() || top() < rect.bottom())
        return false;

    return true;
}

void Rect::render_border(SDL_Renderer *renderer) const
{
    auto rect = to_frect();
    static Color color(Color::random_color());

    SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
    SDL_RenderRect(renderer, &rect);
}

void Rect::render_full(SDL_Renderer *renderer) const
{
    auto rect = to_frect();
    static Color color(Color::random_color());

    SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
    SDL_RenderFillRect(renderer, &rect);
}

Rect &Rect::operator+=(const Vec2 &delta)
{
    m_x += delta.get_x();
    m_y += delta.get_y();
    return *this;
}
Rect Rect::operator+(const Vec2 &delta) const { return Rect(*this).operator+=(delta); }

Rect &Rect::operator-=(const Vec2 &delta)
{
    m_x -= delta.get_x();
    m_y -= delta.get_y();
    return *this;
}
Rect Rect::operator-(const Vec2 &delta) const { return Rect(*this).operator-=(delta); }

Rect &Rect::operator*=(const Vec2 &ratio)
{
    m_width *= ratio.get_x();
    m_height *= ratio.get_y();
    return *this;
}
Rect Rect::operator*(const Vec2 &ratio) const { return Rect(*this).operator*=(ratio); }

Rect &Rect::operator/=(const Vec2 &ratio)
{
    m_width = ratio.get_x() == 0 ? 0 : m_width / ratio.get_x();
    m_height = ratio.get_y() == 0 ? 0 : m_height / ratio.get_y();
    return *this;
}
Rect Rect::operator/(const Vec2 &ratio) const { return Rect(*this).operator/=(ratio); }

float Rect::time_to_collide(const Vec2 &velocity, const Rect &target) const
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