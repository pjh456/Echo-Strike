#include <echo_strike/physics/physical_object.hpp>

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
        box_rect.up() - other_box_rect.down(),
        other_box_rect.up() - box_rect.down());

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
}

void PhysicalObject::handle_collide_object(PhysicalObject &other)
{
    Rect box_rect = box.get_rect();
    Rect other_box_rect = other.get_rect();

    float overlap_x = std::min(
        box_rect.right() - other_box_rect.left(),
        other_box_rect.right() - box_rect.left());
    float overlap_y = std::min(
        box_rect.up() - other_box_rect.down(),
        other_box_rect.up() - box_rect.down());

    if (overlap_x < overlap_y)
    {
        float correction = overlap_x / 2.0f;
        if (box_rect.position().get_x() < other_box_rect.position().get_x())
        {
            box.set_rect(
                {box_rect.left() - correction,
                 box_rect.down(),
                 box_rect.get_width(),
                 box_rect.get_height()});
            other.set_rect(
                {other_box_rect.left() + correction,
                 other_box_rect.down(),
                 other_box_rect.get_width(),
                 other_box_rect.get_height()});
        }
        else
        {
            box.set_rect(
                {box_rect.left() + correction,
                 box_rect.down(),
                 box_rect.get_width(),
                 box_rect.get_height()});
            other.set_rect(
                {other_box_rect.left() - correction,
                 other_box_rect.down(),
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
                 box_rect.down() - correction,
                 box_rect.get_width(),
                 box_rect.get_height()});
            other.set_rect(
                {other_box_rect.left(),
                 other_box_rect.down() + correction,
                 other_box_rect.get_width(),
                 other_box_rect.get_height()});
        }
        else
        {
            box.set_rect(
                {box_rect.left(),
                 box_rect.down() + correction,
                 box_rect.get_width(),
                 box_rect.get_height()});
            other.set_rect(
                {other_box_rect.left(),
                 other_box_rect.down() - correction,
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