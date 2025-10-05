#include <echo_strike/physics/physical_object.hpp>

#include <echo_strike/utils/ray.hpp>

// ====================================================================================
// 段落 1: 构造函数 / 析构函数
// ====================================================================================

PhysicalObject::PhysicalObject()
    : box(*CollisionManager::instance().create_collision_box()),
      m_mess(1.0)
{
    box.set_object(this);
    box.set_src(CollisionLayer::Physics);
    box.add_dst(CollisionLayer::Physics);
    box.add_dst(CollisionLayer::Obstacle);

    // 这个回调函数现在专门用于“碰撞响应”（改变速度）。
    // 它只会在我们将物体移动到精确的碰撞时刻后才被触发。
    box.on_collide(
        [this](CollisionBox &other)
        {
            if (auto wall = dynamic_cast<ObstacleObject *>(other.get_object()))
                handle_collision_response(*wall);
            else if (auto other_obj = dynamic_cast<PhysicalObject *>(other.get_object()))
                handle_collision_response(*other_obj);
        });
}

PhysicalObject::~PhysicalObject()
{
    CollisionManager::instance().destroy_collision_box(&box);
}

// ====================================================================================
// 段落 2: 主更新函数 (逻辑总控)
// 这是更新物理状态的公共入口。
// ====================================================================================

void PhysicalObject::on_update(float delta)
{
    // 连续碰撞检测与响应循环 (CCD)
    // 我们将总时间 `delta` 分解为更小的子步骤，从一个碰撞事件推进到下一个。
    float remaining_time = delta;
    const int max_sub_steps = 10; // 设置安全上限，防止因意外情况导致无限循环

    for (int i = 0; i < max_sub_steps && remaining_time > 1e-6f; ++i)
    {
        sub_step_move(remaining_time);
    }
}

// ====================================================================================
// 段落 3: 子步骤逻辑与辅助函数
// 这些函数实现了核心的CCD循环。
// ====================================================================================

/**
 * @brief 执行单次移动的子步骤。它会找到下一次最早的碰撞，将时间推进到该点，
 *        处理碰撞，然后更新剩余时间。
 * @param remaining_time 引用传递，代表当前更新周期中剩余的时间。此函数会修改它的值。
 */
void PhysicalObject::sub_step_move(float &remaining_time)
{
    // 在剩余时间窗口内，查找最早发生的碰撞。
    auto [time_of_impact, first_collided_box] = find_first_collision(remaining_time);

    // 情况 A: 在剩余时间内不会发生碰撞。
    if (!first_collided_box || time_of_impact > remaining_time)
    {
        // 将物体移动完所有剩余时间，然后结束。
        Object::on_update(remaining_time);
        box.set_rect(m_rect);
        remaining_time = 0.0f; // 发出信号，告知主循环我们已经完成。
    }
    // 情况 B: 找到了一个未来的碰撞。
    else
    {
        // 精确地将物体移动到碰撞发生的那一刻。
        Object::on_update(time_of_impact);
        box.set_rect(m_rect);

        // 现在物体们正好处在完美接触的状态，触发碰撞响应回调函数，
        // 来处理速度上的变化（通过冲量计算）。
        if (box.get_callback() && first_collided_box)
        {
            box.get_callback()(*first_collided_box);
        }

        // 从总时间中减去已经过的时间，让主循环用剩余的时间进行下一次子步骤迭代。
        remaining_time -= time_of_impact;
    }
}

/**
 * @brief 查找第一个将要碰撞的物体以及碰撞时间 (Time of Impact, TOI)。
 * @param max_time 查找碰撞的最大时间范围 (例如，当前帧的剩余时间)。
 * @return 一个包含{碰撞时间, 指向被撞物体的碰撞盒指针}的 pair。
 */
std::pair<float, CollisionBox *> PhysicalObject::find_first_collision(float max_time)
{
    // 宽阶段 (Broad Phase): 创建一个能包围物体整个运动轨迹的包围盒 (AABB)。
    Rect origin_rect = m_rect;
    Vec2 current_speed = get_speed();

    Rect future_rect = origin_rect;
    future_rect.set_x(future_rect.get_x() + current_speed.get_x() * max_time);
    future_rect.set_y(future_rect.get_y() + current_speed.get_y() * max_time);

    // 运动包围盒 (motion_aabb) 应该包含起点和终点两个矩形。
    auto motion_aabb = Rect::bounding_box({origin_rect, future_rect});

    box.set_rect(motion_aabb);
    auto potential_collisions = box.process_collide();
    box.set_rect(origin_rect); // 重要: 探测完毕后，将碰撞盒恢复到物体的实际当前位置。

    // 窄阶段 (Narrow Phase): 在所有潜在的碰撞对象中，精确计算出最早的碰撞时间。
    CollisionBox *first_collided_box = nullptr;
    float first_collided_time = max_time + 1e-6f; // 初始化为一个比最大时间稍大的值

    for (auto other_box : potential_collisions)
    {
        float t = origin_rect.time_to_collide(current_speed, other_box->get_rect());

        // 我们只关心未来的、并且比当前记录的还要早的碰撞。
        // 使用一个极小值 (epsilon, 1e-6f) 来防止与刚刚分离的物体立即再次发生碰撞。
        if (t > 1e-6f && t < first_collided_time)
        {
            first_collided_time = t;
            first_collided_box = other_box;
        }
    }

    return {first_collided_time, first_collided_box};
}

void PhysicalObject::resolve_penetration_pair(ObstacleObject &wall)
{
    Rect box_rect = this->get_rect();
    Rect wall_rect = wall.get_rect();

    Vec2 center_obj = box_rect.position() + box_rect.size() * 0.5f;
    Vec2 center_wall = wall_rect.position() + wall_rect.size() * 0.5f;
    Vec2 n = center_obj - center_wall; // 从墙指向物体的向量

    Vec2 half_size_obj = box_rect.size() * 0.5f;
    Vec2 half_size_wall = wall_rect.size() * 0.5f;
    float overlap_x = (half_size_obj.get_x() + half_size_wall.get_x()) - std::abs(n.get_x());
    float overlap_y = (half_size_obj.get_y() + half_size_wall.get_y()) - std::abs(n.get_y());

    if (overlap_x <= 0 || overlap_y <= 0)
        return;

    Vec2 correction_normal;
    float penetration_depth;
    if (overlap_x < overlap_y)
    {
        penetration_depth = overlap_x;
        // 法线方向应该与 n 的 x 分量同向，将物体推出
        correction_normal = Vec2(n.get_x() > 0 ? 1.0f : -1.0f, 0);
    }
    else
    {
        penetration_depth = overlap_y;
        // 法线方向应该与 n 的 y 分量同向，将物体推出
        correction_normal = Vec2(0, n.get_y() > 0 ? 1.0f : -1.0f);
    }

    const float k_slop = 0.01f; // 允许的微小重叠，防止抖动
    const float percent = 0.8f; // 推开力量的百分比，防止弹射过猛
    Vec2 correction = correction_normal * std::max(penetration_depth - k_slop, 0.0f) * percent;
    // 直接将物体沿修正法线方向移出墙体
    this->set_rect(box_rect + correction);
}

void PhysicalObject::resolve_penetration_pair(PhysicalObject &other)
{
    Rect box_rect = this->get_rect();
    Rect other_box_rect = other.get_rect();

    Vec2 center1 = box_rect.position() + box_rect.size() * 0.5f;
    Vec2 center2 = other_box_rect.position() + other_box_rect.size() * 0.5f;
    Vec2 n = center2 - center1; // 从 this 指向 other 的向量

    if (n.length() < 1e-6f)
        n = Vec2(0, 1.0f); // 处理完美重叠的特殊情况

    Vec2 half_size1 = box_rect.size() * 0.5f;
    Vec2 half_size2 = other_box_rect.size() * 0.5f;
    float overlap_x = (half_size1.get_x() + half_size2.get_x()) - std::abs(n.get_x());
    float overlap_y = (half_size1.get_y() + half_size2.get_y()) - std::abs(n.get_y());

    if (overlap_x <= 0 || overlap_y <= 0)
        return;

    Vec2 correction_normal;
    float penetration_depth;
    // 【关键修正】这里的法线方向必须正确，才能将物体推开而不是推入
    if (overlap_x < overlap_y)
    {
        penetration_depth = overlap_x;
        // 如果 n.x > 0 (other在this右边)，法线应该指向左边 (-1,0) 来推开 this
        // 如果 n.x < 0 (other在this左边)，法线应该指向右边 (1,0) 来推开 this
        correction_normal = Vec2(n.get_x() > 0 ? -1.0f : 1.0f, 0);
    }
    else
    {
        penetration_depth = overlap_y;
        // 同理，修正y轴方向
        correction_normal = Vec2(0, n.get_y() > 0 ? -1.0f : 1.0f);
    }

    const float k_slop = 0.01f;
    const float percent = 0.8f;
    float total_inv_mass = (1.0f / m_mess) + (1.0f / other.m_mess);
    if (total_inv_mass < 1e-6f)
        return; // 如果两个物体都不可移动

    Vec2 correction = correction_normal * (std::max(penetration_depth - k_slop, 0.0f) / total_inv_mass) * percent;

    // 【关键修正】根据修正后的法线方向，正确地移动两个物体
    // this 应该沿着法线方向移动，other 应该沿着反方向移动
    this->set_rect(this->get_rect() + correction * (1.0f / m_mess));
    other.set_rect(other.get_rect() - correction * (1.0f / other.m_mess));
}

// ====================================================================================
// 段落 5: 碰撞响应 (速度更新) - 【最终修正版】
// 这些函数只负责施加冲量来改变速度，不会改变位置。
// ====================================================================================
// ... [前面的代码都一样] ...

void PhysicalObject::handle_collision_response(ObstacleObject &wall)
{
    Rect box_rect = this->get_rect();
    Rect wall_rect = wall.get_rect();

    // --- 【最终版：稳健且简单的法线计算逻辑】 ---
    Vec2 center_obj = box_rect.position() + box_rect.size() * 0.5f;
    Vec2 center_wall = wall_rect.position() + wall_rect.size() * 0.5f;

    // 1. 计算两个物体在X和Y轴上“合并”后的半长和半宽
    float combined_half_w = (box_rect.size().get_x() + wall_rect.size().get_x()) * 0.5f;
    float combined_half_h = (box_rect.size().get_y() + wall_rect.size().get_y()) * 0.5f;

    // 2. 计算中心向量 和 两个轴上的穿透/重叠量
    Vec2 n = center_obj - center_wall; // 从墙指向物体的向量
    float overlap_x = combined_half_w - std::abs(n.get_x());
    float overlap_y = combined_half_h - std::abs(n.get_y());

    Vec2 contact_normal;
    // 3. 比较哪个轴的重叠量更小，这就是碰撞发生的轴 (最小穿透轴)
    if (overlap_x < overlap_y)
    {
        // 碰撞是左右方向的。法线方向就是推开的方向。
        contact_normal = Vec2(n.get_x() > 0 ? 1.0f : -1.0f, 0);
    }
    else
    {
        // 碰撞是上下方向的。
        contact_normal = Vec2(0, n.get_y() > 0 ? 1.0f : -1.0f);
    }

    // --- [标准的冲量计算] ---
    Vec2 u_obj = this->get_speed();
    Vec2 relative_velocity = u_obj; // V_obj - V_wall(0)
    float vel_along_normal = relative_velocity.get_x() * contact_normal.get_x() + relative_velocity.get_y() * contact_normal.get_y();

    // 如果物体速度方向 与 推开它的法线方向 是相反的(点积<0)，说明正在撞过去。
    // 如果是同向的(点积>0)，说明正在远离，无需处理。
    // if (vel_along_normal > 0)
    //     return;

    float restitution = 0.8f;
    // 注意这里的公式，因为 relative_velocity = u_obj，所以 vel_along_normal 是 u_obj 在法线上的投影。
    // 我们需要反转它，所以冲量公式里没有负号。
    float j = -(1 + restitution) * vel_along_normal;
    j /= (1 / m_mess);

    Vec2 impulse = contact_normal * j;

    this->set_speed(u_obj + impulse * (1 / m_mess));
}

void PhysicalObject::handle_collision_response(PhysicalObject &other)
{
    // 这个函数之前的版本是正确的，为了与墙壁碰撞的逻辑统一，我们用同样稳健的算法重写
    Rect box_rect = this->get_rect();
    Rect other_box_rect = other.get_rect();

    Vec2 center1 = box_rect.position() + box_rect.size() * 0.5f;
    Vec2 center2 = other_box_rect.position() + other_box_rect.size() * 0.5f;
    Vec2 n = center2 - center1; // 从 this(1) 指向 other(2) 的向量

    float combined_half_w = (box_rect.size().get_x() + other_box_rect.size().get_x()) * 0.5f;
    float combined_half_h = (box_rect.size().get_y() + other_box_rect.size().get_y()) * 0.5f;

    float overlap_x = combined_half_w - std::abs(n.get_x());
    float overlap_y = combined_half_h - std::abs(n.get_y());

    Vec2 contact_normal;
    // 法线定义为从 this 指向 other
    if (overlap_x < overlap_y)
    {
        contact_normal = Vec2(n.get_x() > 0 ? 1.0f : -1.0f, 0);
    }
    else
    {
        contact_normal = Vec2(0, n.get_y() > 0 ? 1.0f : -1.0f);
    }

    Vec2 u1 = this->get_speed();
    Vec2 u2 = other.get_speed();
    float m1 = this->get_mess();
    float m2 = other.get_mess();

    Vec2 relative_velocity = u2 - u1;
    float vel_along_normal = relative_velocity.get_x() * contact_normal.get_x() + relative_velocity.get_y() * contact_normal.get_y();

    if (vel_along_normal > 0)
        return;

    float restitution = 0.8f;
    float j = -(1 + restitution) * vel_along_normal;
    j /= (1 / m1 + 1 / m2);

    Vec2 impulse = contact_normal * j;
    this->set_speed(u1 - impulse * (1 / m1));
    other.set_speed(u2 + impulse * (1 / m2));
}