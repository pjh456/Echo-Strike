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
    float remaining_time = delta;
    const int max_sub_steps = 25; // 安全措施，防止死循环
    const float epsilon = 1e-5f;

    for (int i = 0; i < max_sub_steps && remaining_time > 1e-6f; ++i)
    {
        float min_time_of_impact = remaining_time;
        PhysicalObject *first_collider = nullptr;
        CollisionBox *first_collided_with = nullptr;

        // 1. 全局查找：在所有物理对象中，找到最早发生的碰撞
        for (auto obj : objs)
        {
            auto [toi, other_box] = obj->find_first_collision(remaining_time);

            // 我们关心的是在剩余时间内、比当前记录还要早的碰撞
            if (other_box && toi < min_time_of_impact)
            {
                min_time_of_impact = toi;
                first_collider = obj;
                first_collided_with = other_box;
            }
        }

        // 2. 推进时间
        // 如果找到的碰撞时间非常小 (几乎为零)，说明物体可能已经重叠或紧贴
        // 为了避免浮点数精度问题导致的无限循环，我们至少推进一个极小的时间步
        // 或者直接处理穿透并继续。这里我们选择推进一个最小步长来避免卡死。
        if (min_time_of_impact < 1e-6f && first_collider)
        {
            // 这种情况通常意味着已经有接触，先处理碰撞响应
            if (auto wall = dynamic_cast<ObstacleObject *>(first_collided_with->get_object()))
            {
                first_collider->resolve_penetration_pair(*wall);
                first_collider->handle_collision_response(*wall);
            }
            else if (auto other_obj = dynamic_cast<PhysicalObject *>(first_collided_with->get_object()))
            {
                first_collider->resolve_penetration_pair(*other_obj);
                first_collider->handle_collision_response(*other_obj);
            }
        }
        else
        {
            // 将所有物体的时间统一推进 min_time_of_impact
            for (auto obj : objs)
            {
                obj->advance_state(min_time_of_impact);
            }
        }

        // 3. 处理碰撞
        if (first_collider && first_collided_with)
        {
            // 此刻，first_collider 和 first_collided_with 的物体正好处在完美接触的状态
            // 调用碰撞响应来更新它们的速度
            if (auto wall = dynamic_cast<ObstacleObject *>(first_collided_with->get_object()))
                first_collider->handle_collision_response(*wall);
            else if (auto other_obj = dynamic_cast<PhysicalObject *>(first_collided_with->get_object()))
                first_collider->handle_collision_response(*other_obj);
        }

        // 4. 更新剩余时间
        remaining_time -= min_time_of_impact;
    }

    // 在整个物理步进循环结束后，作为一个安全网，再处理一遍可能因精度问题残留的穿透
    resolve_all_penetrations();
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