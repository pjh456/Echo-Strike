#ifndef INCLUDE_RECT
#define INCLUDE_RECT

#include <echo_strike/utils/class_marcos.hpp>

#include <echo_strike/utils/vec2.hpp>

#include <SDL3/SDL.h>

#include <ostream>

class Rect
{
private:
    CLASS_PROPERTY(int, x);
    CLASS_PROPERTY(int, y);
    CLASS_PROPERTY(int, width);
    CLASS_PROPERTY(int, height);

public:
    Rect(int, int, int, int);
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
    int left() const { return m_x; }
    int right() const { return m_x + m_width; }
    int up() const { return m_y + m_height; }
    int down() const { return m_y; }

public:
    bool is_inside(const Vec2 &) const;
    bool is_inside(const Rect &) const;

    bool is_on_edge(const Vec2 &) const;
    bool is_on_edge(const Rect &) const;

    bool is_intersect(const Vec2 &) const;
    bool is_intersect(const Rect &) const;

    void render_border(SDL_Renderer *) const;
    void render_full(SDL_Renderer *) const;
};

inline std::ostream &operator<<(std::ostream &os, const Rect &rect)
{
    os << "Rect(" << rect.position() << ", " << rect.position() + rect.size() << ")";
    return os;
}

#endif // INCLUDE_RECT