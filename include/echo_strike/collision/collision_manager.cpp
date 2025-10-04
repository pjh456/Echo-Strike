#include <echo_strike/collision/collision_manager.hpp>

#include <algorithm>
#include <iostream>
#include <utility>

CollisionManager::CollisionManager()
    : m_quad_tree(Rect(0, 0, 800, 600))
{
}

CollisionManager &CollisionManager::instance()
{
    static CollisionManager manager;
    return manager;
}

CollisionBox *CollisionManager::create_collision_box()
{
    auto box = new CollisionBox();
    boxes.push_back(box);
    m_quad_tree.insert(Rect{}, box);
    return box;
}

void CollisionManager::destroy_collision_box(CollisionBox *box)
{
    boxes.erase(
        std::find(
            boxes.begin(),
            boxes.end(),
            box));

    // m_quad_tree.remove(box->get_rect(), box);
    m_quad_tree.remove(box);
    delete box;
}

CollisionManager::~CollisionManager()
{
    for (auto box : boxes)
        delete box;
}

void CollisionManager::process_collide()
{
    for (auto src_box : boxes)
    {
        auto dst_boxes = src_box->process_collide();
        for (auto dst_box : dst_boxes)
        {
            if (dst_box->collide_callback)
                dst_box->collide_callback(*src_box);
        }
    }
}

void CollisionManager::debug_render(SDL_Renderer *renderer) const
{
    for (auto box : boxes)
        box->render_border(renderer);
}