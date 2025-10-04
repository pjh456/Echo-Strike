#ifndef INCLUDE_COLLISION_BOX
#define INCLUDE_COLLISION_BOX

#include <echo_strike/utils/rect.hpp>
#include <echo_strike/utils/color.hpp>

#include <echo_strike/collision/collision_layer.hpp>

#include <SDL3/SDL.h>

#include <functional>
#include <unordered_set>

class CollisionManager;
class Object;

class CollisionBox
{
    friend class CollisionManager;

public:
    using Callback = std::function<void(CollisionBox &)>;
    template <typename T>
    using Set = std::unordered_set<T>;

private:
    Callback collide_callback;

    Set<CollisionLayer> m_dst;

    CLASS_PROPERTY(bool, enable)
    CLASS_PROPERTY(Rect, rect)
    CLASS_PROPERTY(CollisionLayer, src)

    CLASS_POINTER(Object, object)

private:
    CollisionBox()
        : m_object(nullptr)
    {
    }

public:
    ~CollisionBox() = default;
    CollisionBox(const CollisionBox &) = delete;
    CollisionBox &operator=(const CollisionBox &) = delete;

    CollisionBox(CollisionBox &&other) noexcept
        : collide_callback(std::move(other.collide_callback)),
          m_enable(other.m_enable),
          m_src(other.m_src),
          m_dst(std::move(other.m_dst)),
          m_rect(std::move(other.m_rect)),
          m_object(other.m_object)
    {
        other.m_src = CollisionLayer::None;
        other.m_dst.clear();
        other.m_object = nullptr;
    }

    CollisionBox &operator=(CollisionBox &&other) noexcept
    {
        if (this == &other)
            return *this;

        collide_callback = std::move(other.collide_callback);
        m_enable = other.m_enable;
        m_src = other.m_src, other.m_src = CollisionLayer::None;
        m_dst = std::move(other.m_dst), other.m_dst.clear();
        m_rect = std::move(other.m_rect);
        m_object = other.m_object, other.m_object = nullptr;

        return *this;
    }

public:
    void on_collide(Callback &&callback) { collide_callback = std::move(callback); }

    void render_border(SDL_Renderer *renderer) const { m_rect.render_border(renderer); }

    Set<CollisionLayer> &get_dst() { return m_dst; }
    const Set<CollisionLayer> &get_dst() const { return m_dst; }
    void add_dst(CollisionLayer p_dst) { m_dst.insert(p_dst); }
    bool has_dst(CollisionLayer p_dst) const { return m_dst.count(p_dst); }
    void remove_dst(CollisionLayer p_dst) { m_dst.erase(p_dst); }
};

#endif // INCLUDE_COLLISION_BOX