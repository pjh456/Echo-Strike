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
    void on_update(float);

public:
    CollisionBox &collision_box() { return box; }
    const CollisionBox &collision_box() const { return box; }

private:
    void update_recursive(float);
    Vec2 handle_collide_obstacle(ObstacleObject &);
    Vec2 handle_collide_object(PhysicalObject &);
};

#endif // INCLUDE_PHYSICAL_OBJECT