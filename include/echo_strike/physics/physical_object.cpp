#include <echo_strike/physics/physical_object.hpp>

#include <echo_strike/utils/ray.hpp>

PhysicalObject::PhysicalObject()
    : box(*CollisionManager::instance().create_collision_box()),
      m_mess(1.0)
{
    box.set_object(this);
    box.set_src(CollisionLayer::Physics);
    box.add_dst(CollisionLayer::Physics);
    box.add_dst(CollisionLayer::Obstacle);

    box.on_collide(
        [this](CollisionBox &other)
        {
            Rect box_rect = box.get_rect();
            Rect other_box_rect = other.get_rect();

            if (auto wall = dynamic_cast<ObstacleObject *>(other.get_object()))
                handle_collide_obstacle(*wall);
            else if (auto other_obj = dynamic_cast<PhysicalObject *>(other.get_object()))
                handle_collide_object(*other_obj);
        });
}

PhysicalObject::~PhysicalObject()
{
    CollisionManager::instance().destroy_collision_box(&box);
}

void PhysicalObject::on_update(float delta)
{
    is_collided = false;
    update_recursive(delta);
}

void PhysicalObject::update_recursive(float delta)
{
    if (delta <= 1e-6)
        return;

    // ====================================================================
    // 阶段一：位置修正 & 收集接触法线
    // ====================================================================
    std::vector<Vec2> contact_normals;
    int max_separation_iterations = 5;
    for (int i = 0; i < max_separation_iterations; ++i)
    {
        box.set_rect(m_rect);
        auto overlapping_boxes = box.process_collide();

        bool found_overlap = false;
        for (auto other_box : overlapping_boxes)
        {
            if (m_rect.is_intersect(other_box->get_rect()))
            {
                found_overlap = true;
                if (auto wall = dynamic_cast<ObstacleObject *>(other_box->get_object()))
                {
                    // 调用修正后的函数，它会推开物体并返回法线
                    Vec2 normal = handle_collide_obstacle(*wall);
                    if (normal.length() > 0)
                    {
                        contact_normals.push_back(normal);
                    }
                }
                else if (auto obj = dynamic_cast<PhysicalObject *>(other_box->get_object()))
                {
                    Vec2 normal = handle_collide_object(*obj);
                    if (normal.length() > 0)
                    {
                        contact_normals.push_back(normal);
                    }
                }
            }
        }
        if (!found_overlap)
            break;
    }

    // ====================================================================
    // 阶段二：一次性速度响应
    // ====================================================================
    if (!contact_normals.empty())
    {
        // 1. 聚合所有法线，得到平均接触方向
        Vec2 aggregate_normal(0, 0);
        for (const auto &normal : contact_normals)
        {
            aggregate_normal += normal;
        }
        aggregate_normal = aggregate_normal.normalize();

        // 2. 使用物理反射公式计算新速度
        // v_new = v_old - 2 * dot(v_old, n) * n
        Vec2 current_speed = get_speed();
        float dot_product = current_speed.get_x() * aggregate_normal.get_x() + current_speed.get_y() * aggregate_normal.get_y();
        Vec2 reflection = aggregate_normal * (2 * dot_product);
        Vec2 new_speed = current_speed - reflection;

        // 3. ★★★ 进行唯一一次的速度衰减 ★★★
        set_speed(new_speed * 0.7f);
    }

    // ====================================================================
    // 阶段三：未来碰撞预测 (此部分逻辑不变)
    // ====================================================================
    auto origin_rect = m_rect;
    auto origin_speed = get_speed(); // 使用可能已被修正的新速度

    // 2. [广度阶段] 计算运动包围盒并获取潜在碰撞
    Rect future_rect = origin_rect;
    future_rect.set_x(future_rect.get_x() + origin_speed.get_x() * delta);
    future_rect.set_y(future_rect.get_y() + origin_speed.get_y() * delta);

    auto motion_aabb = Rect(
        std::min(origin_rect.left(), future_rect.left()),
        std::min(origin_rect.bottom(), future_rect.bottom()),
        std::max(origin_rect.right(), future_rect.right()) - std::min(origin_rect.left(), future_rect.left()),
        std::max(origin_rect.top(), future_rect.top()) - std::min(origin_rect.bottom(), future_rect.bottom()));

    box.set_rect(motion_aabb);
    auto potential_collisions = box.process_collide();
    box.set_rect(origin_rect);

    // 3. [窄度阶段] 找到路径上的第一次碰撞
    CollisionBox *first_collided_box = nullptr;
    float first_collided_time = delta + 1e-6f;

    for (auto other_box : potential_collisions)
    {
        // 我们只计算未来的碰撞，因为当前的重叠已经在阶段一处理完了
        float t = origin_rect.time_to_collide(origin_speed, other_box->get_rect());
        if (t > 1e-6 && t < first_collided_time)
        {
            first_collided_time = t;
            first_collided_box = other_box;
        }
    }

    // 4. 根据结果处理
    if (!first_collided_box || first_collided_time > delta)
    {
        // 无未来碰撞，直接移动到终点
        Object::on_update(delta);
        box.set_rect(m_rect);
        return;
    }

    // 有未来碰撞
    // a. 移动到碰撞时刻
    Object::on_update(first_collided_time);
    box.set_rect(m_rect);

    // b. 处理该次碰撞
    if (box.get_callback() && first_collided_box)
    {
        box.get_callback()(*first_collided_box);
    }

    // c. 对剩余时间进行递归
    float remaining_time = delta - first_collided_time;
    if (remaining_time > 1e-6)
    {
        update_recursive(remaining_time);
    }
}

// void PhysicalObject::update_recursive(float delta)
// {
//     if (delta <= 1e-6)
//         return;

//     // 1. Record the origin value
//     auto origin_rect = m_rect;
//     auto origin_speed = m_speed;

//     // 2. Try to update totally
//     Object::on_update(delta);

//     // 3. Save the preview value and load the origin one
//     auto future_rect = m_rect;
//     auto future_speed = m_speed;
//     m_rect = origin_rect;
//     m_speed = origin_speed;

//     // 4. Define the motion rays
//     auto
//         top_left_ray = Ray(origin_rect.top_left(), future_rect.top_left()),
//         top_right_ray = Ray(origin_rect.top_right(), future_rect.top_right()),
//         bottom_left_ray = Ray(origin_rect.bottom_left(), future_rect.bottom_left()),
//         bottom_right_ray = Ray(origin_rect.bottom_right(), future_rect.bottom_right());

//     // 4. Calculate the motion rect
//     auto
//         min_x = std::min(origin_rect.left(), future_rect.left()),
//         min_y = std::min(origin_rect.bottom(), future_rect.bottom()),
//         max_x = std::max(origin_rect.right(), future_rect.right()),
//         max_y = std::max(origin_rect.top(), future_rect.top());
//     auto motion_aabb = Rect(min_x, min_y, max_x - min_x, max_y - min_y);

//     // 5. Try to get potential collisions
//     box.set_rect(motion_aabb);
//     auto potential_collisions = box.process_collide();
//     box.set_rect(m_rect);

//     CollisionBox *first_collided_box = nullptr;
//     float first_collided_time = delta + 1e-6f;
//     for (auto other_box : potential_collisions)
//     {
//         float t = origin_rect.time_to_collide(origin_speed, other_box->get_rect());
//         if (t <= 1e6 && is_collided)
//             continue;
//         if (t >= 0 && t < first_collided_time)
//         {
//             first_collided_time = t;
//             first_collided_box = other_box;
//         }
//     }

//     if (!first_collided_box || first_collided_time > delta)
//     {
//         Object::on_update(delta); // 使用基类方法一次性移动
//         box.set_rect(m_rect);
//         return;
//     }

//     if (first_collided_time <= 1e-6)
//     {
//         if (this->is_collided)
//             return;

//         // 这是本帧第一次处理瞬时碰撞
//         if (box.get_callback() && first_collided_box)
//         {
//             box.get_callback()(*first_collided_box); // 处理碰撞（推开物体、更新速度）
//         }

//         this->is_collided = true;

//         update_recursive(delta);
//     }
//     else
//     {
//         Object::on_update(first_collided_time);
//         box.set_rect(m_rect);

//         if (box.get_callback() && first_collided_box)
//         {
//             box.get_callback()(*first_collided_box);
//         }

//         float remaining_time = delta - first_collided_time;
//         if (remaining_time > 1e-6)
//         {
//             update_recursive(remaining_time);
//         }
//     }
// }

Vec2 PhysicalObject::handle_collide_obstacle(ObstacleObject &other)
{
    if (m_speed.get_x() == 0 || m_speed.get_y() == 0)
    {
        set_speed(-m_speed);
        return Vec2(0, 0);
    }

    Rect box_rect = box.get_rect();
    Rect other_box_rect = other.get_rect();

    float overlap_x = std::min(
        box_rect.right() - other_box_rect.left(),
        other_box_rect.right() - box_rect.left());
    float overlap_y = std::min(
        box_rect.top() - other_box_rect.bottom(),
        other_box_rect.top() - box_rect.bottom());

    if (overlap_x == overlap_y)
    {
        set_speed(-get_speed());
        return Vec2(1, 1);
    }

    Vec2 contact_normal(0, 0);

    if (overlap_x < overlap_y)
    {
        if (m_speed.get_x() > 0)
        {
            box_rect.set_x(box_rect.get_x() - overlap_x);
            contact_normal = Vec2(-1, 0);
        }
        else
        {
            box_rect.set_x(box_rect.get_x() + overlap_x);
            contact_normal = Vec2(1, 0);
        }
        set_speed(Vec2(-m_speed.get_x(), m_speed.get_y()));
    }
    else
    {
        if (m_speed.get_y() > 0)
        {
            box_rect.set_y(box_rect.get_y() - overlap_y);
            contact_normal = Vec2(0, -1);
        }
        else
        {
            box_rect.set_y(box_rect.get_y() + overlap_y);
            contact_normal = Vec2(0, 1);
        }
        set_speed(Vec2(m_speed.get_x(), -m_speed.get_y()));
    }

    box.set_rect(box_rect);

    return contact_normal;
}

Vec2 PhysicalObject::handle_collide_object(PhysicalObject &other)
{
    Rect box_rect = box.get_rect();
    Rect other_box_rect = other.get_rect();

    float overlap_x = std::min(
        box_rect.right() - other_box_rect.left(),
        other_box_rect.right() - box_rect.left());
    float overlap_y = std::min(
        box_rect.top() - other_box_rect.bottom(),
        other_box_rect.top() - box_rect.bottom());

    Vec2 contact_normal(0, 0);

    if (overlap_x < overlap_y)
    {
        float correction = overlap_x / 2.0f;
        if (box_rect.position().get_x() < other_box_rect.position().get_x())
        {
            box.set_rect(
                {box_rect.left() - correction,
                 box_rect.bottom(),
                 box_rect.get_width(),
                 box_rect.get_height()});
            other.set_rect(
                {other_box_rect.left() + correction,
                 other_box_rect.bottom(),
                 other_box_rect.get_width(),
                 other_box_rect.get_height()});
            contact_normal = Vec2(-1, 0);
        }
        else
        {
            box.set_rect(
                {box_rect.left() + correction,
                 box_rect.bottom(),
                 box_rect.get_width(),
                 box_rect.get_height()});
            other.set_rect(
                {other_box_rect.left() - correction,
                 other_box_rect.bottom(),
                 other_box_rect.get_width(),
                 other_box_rect.get_height()});
            contact_normal = Vec2(1, 0);
        }
    }
    else
    {
        float correction = overlap_y / 2.0f;
        if (box_rect.position().get_y() < other_box_rect.position().get_y())
        {
            box.set_rect(
                {box_rect.left(),
                 box_rect.bottom() - correction,
                 box_rect.get_width(),
                 box_rect.get_height()});
            other.set_rect(
                {other_box_rect.left(),
                 other_box_rect.bottom() + correction,
                 other_box_rect.get_width(),
                 other_box_rect.get_height()});
            contact_normal = Vec2(0, -1);
        }
        else
        {
            box.set_rect(
                {box_rect.left(),
                 box_rect.bottom() + correction,
                 box_rect.get_width(),
                 box_rect.get_height()});
            other.set_rect(
                {other_box_rect.left(),
                 other_box_rect.bottom() - correction,
                 other_box_rect.get_width(),
                 other_box_rect.get_height()});
            contact_normal = Vec2(0, 1);
        }
    }

    auto u1 = m_speed, u2 = other.m_speed;
    auto m1 = m_mess, m2 = other.m_mess;

    auto v1 = ((m1 - m2) * u1 + 2 * m2 * u2) / (m1 + m2);
    auto v2 = ((m2 - m1) * u2 + 2 * m1 * u1) / (m1 + m2);

    set_speed(v1);
    other.set_speed(v2);

    return contact_normal;
}