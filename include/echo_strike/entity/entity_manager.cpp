#include <echo_strike/entity/entity_manager.hpp>

#include <algorithm>

EntityManager &EntityManager::instance()
{
    static EntityManager manager;
    return manager;
}

void EntityManager::destroy_entity(Entity *entity)
{
    if (entity == nullptr)
        return;

    auto hit_box = entity->get_hit_box();
    CollisionManager::instance().destroy_collision_box(hit_box);

    entities.erase(std::find_if(
        entities.begin(),
        entities.end(),
        [&entity](Entity *other)
        {
            return other == entity;
        }));

    delete entity;
}