#ifndef INCLUDE_COLLISION_BOX
#define INCLUDE_COLLISION_BOX

#include <echo_strike/utils/rect.hpp>
#include <echo_strike/utils/color.hpp>

#include <echo_strike/collision/collision_layer.hpp>

#include <SDL3/SDL.h>

#include <functional>

class CollisionManager;
class Entity;

class CollisionBox
{
    friend class CollisionManager;

public:
    using Callback = std::function<void(Entity *)>;

private:
    Callback collide_callback;

    CLASS_PROPERTY(bool, enable);
    CLASS_PROPERTY(Rect, rect)
    CLASS_PROPERTY(CollisionLayer, src)
    CLASS_PROPERTY(CollisionLayer, dst)

    CLASS_POINTER(Entity, entity);

private:
    CollisionBox() = default;

public:
    ~CollisionBox() = default;
    CollisionBox(const CollisionBox &) = default;
    CollisionBox(CollisionBox &&) = default;
    DEFAULT_EQUAL_OPERATOR(CollisionBox)

public:
    void on_collide(Callback &&callback) { collide_callback = std::move(callback); }

    void render_border(SDL_Renderer *renderer) const { m_rect.render_border(renderer); }
};

#endif // INCLUDE_COLLISION_BOX