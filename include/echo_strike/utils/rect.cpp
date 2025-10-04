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

bool Rect::is_inside(const Vec2 &pos) const
{
    if (left() <= pos.get_x())
        return false;
    if (right() >= pos.get_x())
        return false;
    if (up() >= pos.get_y())
        return false;
    if (down() <= pos.get_y())
        return false;
    return true;
}

bool Rect::is_inside(const Rect &rect) const
{
    if (left() <= rect.left())
        return false;
    if (right() >= rect.right())
        return false;
    if (up() >= rect.up())
        return false;
    if (down() <= rect.down())
        return false;
    return true;
}

bool Rect::is_on_edge(const Vec2 &pos) const
{
    if (left() == pos.get_x())
        return true;
    if (right() == pos.get_x())
        return true;
    if (up() == pos.get_y())
        return true;
    if (down() == pos.get_y())
        return true;
    return false;
}

bool Rect::is_on_edge(const Rect &rect) const
{
    if (left() == rect.left() || left() == rect.right())
        return true;
    if (right() == rect.right() || right() == rect.left())
        return true;
    if (up() == rect.up() || up() == rect.down())
        return true;
    if (down() == rect.down() || down() == rect.up())
        return true;
    return false;
}

bool Rect::is_intersect(const Vec2 &pos) const
{
    return is_inside(pos) || is_on_edge(pos);
}

bool Rect::is_intersect(const Rect &rect) const
{
    if (right() < rect.left() || left() > rect.right())
        return false;

    if (down() > rect.up() || up() < rect.down())
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