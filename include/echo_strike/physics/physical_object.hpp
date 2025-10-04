#ifndef INCLUDE_PHYSICAL_OBJECT
#define INCLUDE_PHYSICAL_OBJECT

#include <echo_strike/physics/object.hpp>
#include <echo_strike/physics/obstacle_object.hpp>

#include <echo_strike/collision/collision_layer.hpp>
#include <echo_strike/collision/collision_box.hpp>
#include <echo_strike/collision/collision_manager.hpp>

#include <iostream>

class PhysicalObject : public Object
{
private:
    CollisionBox &box;
    CLASS_PROPERTY(float, mess);
    bool is_collided = false;

public:
    PhysicalObject();

    ~PhysicalObject();

public:
    void on_update(float delta)
    {
        auto origin_rect = m_rect;
        auto origin_speed = m_speed;
        Object::on_update(delta);
        box.set_rect(m_rect);

        auto dst_boxes = box.process_collide();
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

        if (first_collide_time > delta || is_collided)
        {
            is_collided = false;
            return;
        }

        is_collided = true;
        m_rect = origin_rect, m_speed = origin_speed;
        Object::on_update(first_collide_time);
        box.get_callback()(*first_collide_box);
        on_update(delta - first_collide_time);
    }

public:
    CollisionBox &collision_box() { return box; }
    const CollisionBox &collision_box() const { return box; }

private:
    void handle_collide_obstacle(ObstacleObject &);
    void handle_collide_object(PhysicalObject &);
};

#endif // INCLUDE_PHYSICAL_OBJECT