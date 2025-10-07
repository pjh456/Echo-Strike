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
    Rect m_rect;

    CLASS_PROPERTY(bool, enable)

    CLASS_PROPERTY(CollisionLayer, src)

    Object *m_object;

private:
    CollisionBox();

public:
    ~CollisionBox() = default;
    CollisionBox(const CollisionBox &) = delete;
    CollisionBox &operator=(const CollisionBox &) = delete;

    CollisionBox(CollisionBox &&) noexcept;

    CollisionBox &operator=(CollisionBox &&) noexcept;

public:
    void on_collide(Callback &&callback) { collide_callback = std::move(callback); }
    Callback get_callback() const { return collide_callback; }

    void render_border(SDL_Renderer *renderer) const { m_rect.render_border(renderer); }

    std::vector<CollisionBox *> process_collide() const;

public:
    Set<CollisionLayer> &get_dst() { return m_dst; }
    const Set<CollisionLayer> &get_dst() const { return m_dst; }
    void add_dst(CollisionLayer p_dst) { m_dst.insert(p_dst); }
    bool has_dst(CollisionLayer p_dst) const { return m_dst.count(p_dst); }
    void remove_dst(CollisionLayer p_dst) { m_dst.erase(p_dst); }

    Rect get_rect() const { return m_rect; }
    void set_rect(const Rect &rect);

    Object *get_object() const { return m_object; }
    void set_object(Object *obj)
    {
        if (m_object)
            delete m_object;
        m_object = obj;
    }
};

#endif // INCLUDE_COLLISION_BOX