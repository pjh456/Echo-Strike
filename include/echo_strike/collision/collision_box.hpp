#ifndef INCLUDE_COLLISION_BOX
#define INCLUDE_COLLISION_BOX

#include <echo_strike/utils/rect.hpp>
#include <echo_strike/utils/color.hpp>

#include <echo_strike/collision/collision_layer.hpp>

#include <SDL3/SDL.h>

#include <functional>

class CollisionManager;

class CollisionBox
{
    friend class CollisionManager;

private:
    std::function<void()> collide_callback;

    CLASS_PROPERTY(bool, enable);
    CLASS_PROPERTY(Rect, rect)
    CLASS_PROPERTY(CollisionLayer, src)
    CLASS_PROPERTY(CollisionLayer, dst)

private:
    CollisionBox() = default;

public:
    ~CollisionBox() = default;
    CollisionBox(const CollisionBox &) = default;
    CollisionBox(CollisionBox &&) = default;
    DEFAULT_EQUAL_OPERATOR(CollisionBox)

public:
    void on_collide(std::function<void()> &&callback) { collide_callback = callback; }

    void render_border(SDL_Renderer *renderer) const
    {
        auto rect = m_rect.to_frect();
        // static Color color = Color::random_color();
        static Color color(Color::random_color());

        SDL_SetRenderDrawColor(renderer, color.R, color.G, color.B, color.A);
        SDL_RenderRect(renderer, &rect);
    }
};

#endif // INCLUDE_COLLISION_BOX