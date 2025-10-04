#ifndef INCLUDE_COLLISION_MANAGER
#define INCLUDE_COLLISION_MANAGER

#include <echo_strike/collision/collision_box.hpp>

#include <SDL3/SDL.h>

#include <vector>

class CollisionManager
{
public:
    static CollisionManager &instance();
    CollisionBox *create_collision_box();
    void destroy_collision_box(CollisionBox *);

    void debug_render(SDL_Renderer *) const;

private:
    std::vector<CollisionBox> boxes;

public:
    CollisionManager() = default;
    ~CollisionManager() = default;

public:
    size_t size() const { return boxes.size(); }
    void clear() { boxes.clear(); }

    void process_collide();
};

#endif // INCLUDE_COLLISION_MANAGER