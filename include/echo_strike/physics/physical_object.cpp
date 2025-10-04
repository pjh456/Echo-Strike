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

    // 1. Record the origin value
    auto origin_rect = m_rect;
    auto origin_speed = m_speed;

    // 2. Try to update totally
    Object::on_update(delta);

    // 3. Save the preview value and load the origin one
    auto future_rect = m_rect;
    auto future_speed = m_speed;
    m_rect = origin_rect;
    m_speed = origin_speed;

    // 4. Define the motion rays
    auto
        top_left_ray = Ray(origin_rect.top_left(), future_rect.top_left()),
        top_right_ray = Ray(origin_rect.top_right(), future_rect.top_right()),
        bottom_left_ray = Ray(origin_rect.bottom_left(), future_rect.bottom_left()),
        bottom_right_ray = Ray(origin_rect.bottom_right(), future_rect.bottom_right());

    // 4. Calculate the motion rect
    auto
        min_x = std::min(origin_rect.left(), future_rect.left()),
        min_y = std::min(origin_rect.bottom(), future_rect.bottom()),
        max_x = std::max(origin_rect.right(), future_rect.right()),
        max_y = std::max(origin_rect.top(), future_rect.top());
    auto motion_aabb = Rect(min_x, min_y, max_x - min_x, max_y - min_y);

    // 5. Try to get potential collisions
    box.set_rect(motion_aabb);
    auto potential_collisions = box.process_collide();
    box.set_rect(m_rect);

    CollisionBox *first_collided_box = nullptr;
    float first_collided_time = delta + 1e-6f;
    for (auto other_box : potential_collisions)
    {
        float t = origin_rect.time_to_collide(origin_speed, other_box->get_rect());
        if (t <= 1e6 && is_collided)
            continue;
        if (t >= 0 && t < first_collided_time)
        {
            first_collided_time = t;
            first_collided_box = other_box;
        }
    }

    if (!first_collided_box || first_collided_time > delta)
    {
        Object::on_update(delta); // 使用基类方法一次性移动
        box.set_rect(m_rect);
        return;
    }

    if (first_collided_time <= 1e-6)
    {
        if (this->is_collided)
            return;

        // 这是本帧第一次处理瞬时碰撞
        if (box.get_callback() && first_collided_box)
        {
            box.get_callback()(*first_collided_box); // 处理碰撞（推开物体、更新速度）
        }

        this->is_collided = true;

        update_recursive(delta);
    }
    else
    {
        Object::on_update(first_collided_time);
        box.set_rect(m_rect);

        if (box.get_callback() && first_collided_box)
        {
            box.get_callback()(*first_collided_box);
        }

        float remaining_time = delta - first_collided_time;
        if (remaining_time > 1e-6)
        {
            update_recursive(remaining_time);
        }
    }
}

void PhysicalObject::handle_collide_obstacle(ObstacleObject &other)
{
    if (m_speed.get_x() == 0 || m_speed.get_y() == 0)
    {
        set_speed(-m_speed);
        return;
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
        return;
    }

    if (overlap_x < overlap_y)
    {
        if (m_speed.get_x() > 0)
            box_rect.set_x(box_rect.get_x() - overlap_x);
        else
            box_rect.set_x(box_rect.get_x() + overlap_x);
        set_speed(Vec2(-m_speed.get_x(), m_speed.get_y()));
    }
    else
    {
        if (m_speed.get_y() > 0)
            box_rect.set_y(box_rect.get_y() - overlap_y);
        else
            box_rect.set_y(box_rect.get_y() + overlap_y);
        set_speed(Vec2(m_speed.get_x(), -m_speed.get_y()));
    }
    box.set_rect(box_rect);

    set_speed(get_speed() * 0.9);
}

void PhysicalObject::handle_collide_object(PhysicalObject &other)
{
    Rect box_rect = box.get_rect();
    Rect other_box_rect = other.get_rect();

    float overlap_x = std::min(
        box_rect.right() - other_box_rect.left(),
        other_box_rect.right() - box_rect.left());
    float overlap_y = std::min(
        box_rect.top() - other_box_rect.bottom(),
        other_box_rect.top() - box_rect.bottom());

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
        }
    }

    auto u1 = m_speed, u2 = other.m_speed;
    auto m1 = m_mess, m2 = other.m_mess;

    auto v1 = ((m1 - m2) * u1 + 2 * m2 * u2) / (m1 + m2);
    auto v2 = ((m2 - m1) * u2 + 2 * m1 * u1) / (m1 + m2);

    set_speed(v1);
    other.set_speed(v2);
}