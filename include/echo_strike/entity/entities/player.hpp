#ifndef INCLUDE_PLAYER
#define INCLUDE_PLAYER

#include <echo_strike/entity/entity.hpp>
#include <echo_strike/entity/entity_state.hpp>
#include <echo_strike/device/input_manager.hpp>
#include <echo_strike/config/resource_manager.hpp>
#include <echo_strike/device/device_manager.hpp>

class Player : public Entity
{
private:
    const float SPEED_RUN = 300.0f;
    const float SPEED_JUMP = 780.0f;
    const float SPEED_ROLL = 800.0f;

public:
    Player()
    {
        auto &manager = ResourceManager::instance();
        auto renderer = DeviceManager::instance().get_renderer();

        auto idle_anim = new EntityState();
        auto [idle_count, idle_atlas] =
            manager.load_atlas(
                renderer->get_renderer(),
                "player/idle");
        idle_anim->get_anim().add_frames(*idle_atlas);
        anim_sm.register_state("idle", idle_anim);
    }

    ~Player()
    {
    }

public:
    void on_update(float delta) override
    {
        Entity::on_update(delta);
        if (m_rect.top() == FLOOR)
            anim_sm.switch_to("idle");
    }
};

#endif // INCLUDE_PLAYER