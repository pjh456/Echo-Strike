#include <echo_strike/entity/entity.hpp>

#include <echo_strike/collision/collision_manager.hpp>

#include <echo_strike/config/config_manager.hpp>
#include <echo_strike/config/resource_manager.hpp>

#include <pjh_json/helpers/json_ref.hpp>

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
    if (std::abs(m_speed.get_x()) > 1e-6)
        m_is_moving = true;
    else
        m_is_moving = false;

    Object::on_update(ms);

    if (m_rect.top() >= m_boundary.top())
        m_is_on_floor = true;
    else if (m_speed.get_y() >= 0)
        m_is_falling = true;
    else
        m_is_jumping = true;

    anim_sm.on_update(ms);
}

void Entity::on_render(Renderer *renderer)
{
    auto raw_ptr = anim_sm.get_current_state();
    if (!raw_ptr)
        return;

    if (auto ptr = dynamic_cast<EntityState *>(raw_ptr))
    {
        ptr->get_anim().set_position(m_rect.get_position());

        auto frame_size = ptr->get_anim().get_current_frame().src.get_size();
        m_rect.set_size(frame_size);

        auto hurt_box_rect = m_hurt_box->get_rect();
        hurt_box_rect.set_size(frame_size);
        hurt_box_rect.set_position(m_rect.get_position());
        m_hurt_box->set_rect(hurt_box_rect);

        ptr->get_anim().render(renderer);
    }
}

void Entity::load_animations(const std::string &key)
{
    auto &config = ConfigManager::instance().get_config();
    auto anims = config["animations"][key].get()->as_array()->as_vector();

    for (auto anim : anims)
    {
        auto obj = pjh_std::json::Ref(anim->as_object());
        auto [anim_key, anim_cache] = ResourceManager::instance().load_atlas_cache(obj["key"].as_str());
        if (anim_cache)
        {
            auto anim_node = new EntityState(this);
            anim_node->get_anim().add_frames(*anim_cache);
            anim_node->get_anim().set_interval(obj["interval"].as_float());
            anim_sm.register_state(obj["name"].as_str(), anim_node);
        }
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