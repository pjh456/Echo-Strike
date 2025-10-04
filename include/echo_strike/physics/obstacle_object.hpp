#ifndef INCLUDE_OBSTACLE_OBJECT
#define INCLUDE_OBSTACLE_OBJECT

#include <echo_strike/physics/object.hpp>

#include <echo_strike/collision/collision_layer.hpp>
#include <echo_strike/collision/collision_box.hpp>
#include <echo_strike/collision/collision_manager.hpp>

class ObstacleObject : public Object
{
private:
    CollisionBox &box;

public:
    ObstacleObject()
        : box(*CollisionManager::instance().create_collision_box())
    {
        box.set_object(this);
        box.set_src(CollisionLayer::Obstacle);
    }

    ~ObstacleObject()
    {
        CollisionManager::instance().destroy_collision_box(&box);
    }

public:
    void set_rect(const Rect &rect)
    {
        box.set_rect(rect);
        Object::set_rect(rect);
    }
};

#endif // INCLUDE_OBSTACLE_OBJECT