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
    if (delta <= 1e-6)
        return;

    auto origin_rect = m_rect;
    auto origin_speed = m_speed;

    Object::on_update(delta);

    // 原始矩形角点
    auto tl0 = origin_rect.top_left();
    auto tr0 = origin_rect.top_right();
    auto bl0 = origin_rect.bottom_left();
    auto br0 = origin_rect.bottom_right();

    // 更新后矩形角点
    auto tl1 = m_rect.top_left();
    auto tr1 = m_rect.top_right();
    auto bl1 = m_rect.bottom_left();
    auto br1 = m_rect.bottom_right();

    // 计算 motion AABB
    float
        min_x =
            std::min({tl0.get_x(), tr0.get_x(), bl0.get_x(), br0.get_x(),
                      tl1.get_x(), tr1.get_x(), bl1.get_x(), br1.get_x()}),
        max_x =
            std::max({tl0.get_x(), tr0.get_x(), bl0.get_x(), br0.get_x(),
                      tl1.get_x(), tr1.get_x(), bl1.get_x(), br1.get_x()}),
        min_y =
            std::min({tl0.get_y(), tr0.get_y(), bl0.get_y(), br0.get_y(),
                      tl1.get_y(), tr1.get_y(), bl1.get_y(), br1.get_y()}),
        max_y =
            std::max({tl0.get_y(), tr0.get_y(), bl0.get_y(), br0.get_y(),
                      tl1.get_y(), tr1.get_y(), bl1.get_y(), br1.get_y()});

    // motion 包围盒
    Rect motion_aabb(min_x, min_y, max_x - min_x, max_y - min_y);

    box.set_rect(motion_aabb);
    auto motion_boxes = box.process_collide();
    box.set_rect(m_rect);

    if (motion_boxes.empty())
        return;

    std::vector<CollisionBox *> dst_boxes;
    dst_boxes.reserve(motion_boxes.size());

    for (auto motion_box : motion_boxes)
    {
        if (m_rect.is_intersect(motion_box->get_rect()))
            dst_boxes.push_back(motion_box);
    }

    if (dst_boxes.empty())
        return;

    auto first_collide_box = dst_boxes.front();
    auto first_collide_time = m_rect.time_to_collide(origin_speed, first_collide_box->get_rect());

    for (auto dst : dst_boxes)
    {
        auto current_collide_time = m_rect.time_to_collide(origin_speed, dst->get_rect());
        if (current_collide_time < first_collide_time)
        {
            first_collide_box = dst;
            first_collide_time = current_collide_time;
        }
    }

    if (first_collide_time >= delta - 1e-6f)
        return;

    m_rect = origin_rect, m_speed = origin_speed;
    Object::on_update(first_collide_time);
    box.get_callback()(*first_collide_box);
    on_update(delta - first_collide_time);
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