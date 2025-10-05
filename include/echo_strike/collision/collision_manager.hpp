#ifndef INCLUDE_COLLISION_MANAGER
#define INCLUDE_COLLISION_MANAGER

#include <echo_strike/utils/quadtree.hpp>
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
    std::vector<CollisionBox *> boxes;
    QuadTree<CollisionBox> m_quad_tree;

private:
    CollisionManager();
    ~CollisionManager();

public:
    size_t size() const { return boxes.size(); }
    void clear();

    std::vector<CollisionBox *> &collision_boxes() { return boxes; }
    const std::vector<CollisionBox *> &collision_boxes() const { return boxes; }

    QuadTree<CollisionBox> &quad_tree() { return m_quad_tree; }
    const QuadTree<CollisionBox> &quad_tree() const { return m_quad_tree; }

public:
    void process_collide();
};

#endif // INCLUDE_COLLISION_MANAGER