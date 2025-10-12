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

void Entity::on_update(float ms)
{
    Object::on_update(ms);
    anim_sm.on_update(ms);
}

void Entity::on_render(Renderer *renderer)
{
    auto raw_ptr = anim_sm.get_current_state();
    if (!raw_ptr)
        return;

    if (auto ptr = dynamic_cast<EntityState *>(raw_ptr))
    {
        auto frame_size = ptr->get_anim().get_current_frame().src.get_size();
        m_rect.set_size(frame_size);

        auto hurt_box_rect = m_hurt_box->get_rect();
        hurt_box_rect.set_size(frame_size);
        hurt_box_rect.set_position(m_rect.get_position());
        m_hurt_box->set_rect(hurt_box_rect);

        ptr->get_anim().render(renderer);
    }
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