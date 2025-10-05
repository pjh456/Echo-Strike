#include <echo_strike/physics/physics_manager.hpp>
#include <echo_strike/physics/physical_object.hpp>

#include <algorithm>

#include <SDL3/SDL_render.h>

PhysicsManager &PhysicsManager::instance()
{
    static PhysicsManager manager;
    return manager;
}

PhysicalObject *PhysicsManager::create_physical_object()
{
    auto obj = new PhysicalObject();
    objs.push_back(obj);
    return obj;
}

void PhysicsManager::destroy_physical_object(PhysicalObject *obj)
{
    objs.erase(
        std::find(
            objs.begin(),
            objs.end(),
            obj));

    delete obj;
}

void PhysicsManager::on_update(float delta)
{
    resolve_all_penetrations();

    for (auto obj : objs)
    {
        obj->on_update(delta);
    }
}

void PhysicsManager::render(SDL_Renderer *renderer)
{
    for (auto *p : objs)
        p->render_full(renderer);
}

PhysicsManager::~PhysicsManager()
{
    clear();
}

void PhysicsManager::clear()
{
    auto destroy_objects = objs;
    for (auto obj : destroy_objects)
        destroy_physical_object(obj);
    objs.clear();
}

void PhysicsManager::resolve_all_penetrations()
{
    const int max_iterations = 10;
    for (int i = 0; i < max_iterations; ++i)
    {
        was_any_overlap_found = false;
        for (auto obj : objs)
        {
            auto &box = obj->collision_box();
            auto overlapping_boxes = box.process_collide();

            for (auto other_box : overlapping_boxes)
            {
                handle_collision(box, other_box);
            }
        }
        if (!was_any_overlap_found)
            break;
    }
}

void PhysicsManager::handle_collision(CollisionBox &src, CollisionBox *dst)
{
    if (dst == nullptr)
        return;

    if (!src.get_rect().is_intersect(dst->get_rect()))
        return;

    auto obj = dynamic_cast<PhysicalObject *>(src.get_object());

    auto other_obj_base = dst->get_object();
    if (!other_obj_base)
        return;

    if (auto wall = dynamic_cast<ObstacleObject *>(other_obj_base))
    {
        obj->resolve_penetration_pair(*wall);
        was_any_overlap_found = true;
    }
    else if (auto other_obj = dynamic_cast<PhysicalObject *>(other_obj_base))
    {
        if (obj < other_obj)
        {
            obj->resolve_penetration_pair(*other_obj);
            was_any_overlap_found = true;
        }
    }
}