#include <echo_strike/entity/entity.hpp>

#include <echo_strike/collision/collision_manager.hpp>

Entity::Entity()
{
    m_hit_box = CollisionManager::instance().create_collision_box();
    m_hit_box->set_enable(false);
    m_hurt_box = CollisionManager::instance().create_collision_box();
}

Entity::~Entity()
{
    CollisionManager::instance().destroy_collision_box(m_hit_box);
    CollisionManager::instance().destroy_collision_box(m_hurt_box);
}
void Entity::set_hit_box(CollisionBox *box)
{
    if (m_hit_box)
        CollisionManager::instance().destroy_collision_box(m_hit_box);
    m_hit_box = box;
}

void Entity::set_hurt_box(CollisionBox *box)
{
    if (m_hurt_box)
        CollisionManager::instance().destroy_collision_box(m_hurt_box);
    m_hurt_box = box;
}