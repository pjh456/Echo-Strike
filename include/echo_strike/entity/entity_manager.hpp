#ifndef INCLUDE_ENTITY_MANAGER
#define INCLUDE_ENTITY_MANAGER

#include <echo_strike/entity/entity.hpp>
#include <echo_strike/collision/collision_manager.hpp>

#include <vector>

class EntityManager
{
public:
    static EntityManager &instance();

    template <typename T>
    Entity *create_entity()
    {
        auto collision_box = CollisionManager::instance().create_collision_box();
        return static_cast<Entity *>(new T(collision_box));
    }

    void destroy_entity(Entity *);

private:
    std::vector<Entity *> entities;

public:
};

#endif // INCLUDE_ENTITY_MANAGER