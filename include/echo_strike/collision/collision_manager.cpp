#include <echo_strike/collision/collision_manager.hpp>

#include <algorithm>

CollisionManager &CollisionManager::instance()
{
    static CollisionManager manager;
    return manager;
}

CollisionBox *CollisionManager::create_collision_box()
{
    CollisionBox temp;
    boxes.push_back(temp);
    return &boxes.back();
}

void CollisionManager::destroy_collision_box(CollisionBox *box)
{
    boxes.erase(
        std::find_if(
            boxes.begin(),
            boxes.end(),
            [box](CollisionBox &other)
            { return &other == box; }));
}

void CollisionManager::process_collide()
{
    for (auto &src_box : boxes)
    {
        if (!src_box.get_enable())
            continue;

        if (src_box.get_src() == CollisionLayer::None || src_box.get_dst().empty())
            continue;

        for (auto &dst_box : boxes)
        {
            if ((&src_box == &dst_box) || (!src_box.has_dst(dst_box.get_src())))
                continue;

            if (!dst_box.get_enable())
                continue;

            if (dst_box.get_src() == CollisionLayer::None)
                continue;

            if (dst_box.collide_callback && dst_box.m_rect.is_intersect(src_box.m_rect))
                dst_box.collide_callback(src_box);
        }
    }
}

void CollisionManager::debug_render(SDL_Renderer *renderer) const
{
    for (auto &box : boxes)
        box.render_border(renderer);
}