#ifndef INCLUDE_PHYSICAL_OBJECT
#define INCLUDE_PHYSICAL_OBJECT

#include <echo_strike/physics/object.hpp>
#include <echo_strike/physics/obstacle_object.hpp>

#include <echo_strike/collision/collision_layer.hpp>
#include <echo_strike/collision/collision_box.hpp>
#include <echo_strike/collision/collision_manager.hpp>

#include <iostream>

class PhysicsManager;

class PhysicalObject : public Object
{
    friend PhysicsManager;

private:
    CollisionBox &box;
    bool is_collided = false;

public:
    PhysicalObject();

    ~PhysicalObject();

public:
    void advance_state(float time_step);

public:
    CollisionBox &collision_box() { return box; }
    const CollisionBox &collision_box() const { return box; }

private:
    std::pair<float, CollisionBox *> find_first_collision(float);
    void resolve_penetration_pair(ObstacleObject &);
    void resolve_penetration_pair(PhysicalObject &);

    void handle_collision_response(ObstacleObject &);
    void handle_collision_response(PhysicalObject &);
};

#endif // INCLUDE_PHYSICAL_OBJECT