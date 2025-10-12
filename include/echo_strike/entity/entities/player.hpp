#ifndef INCLUDE_PLAYER
#define INCLUDE_PLAYER

#include <echo_strike/entity/entity.hpp>
#include <echo_strike/entity/entity_state.hpp>

#include <echo_strike/device/input_manager.hpp>
#include <echo_strike/device/device_manager.hpp>

#include <echo_strike/config/resource_manager.hpp>
#include <echo_strike/config/config_manager.hpp>

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

        init_move_controller();
        load_animations("player");
    }

    ~Player()
    {
        Entity::~Entity();
    }

public:
    void on_update(float ms) { Entity::on_update(ms); }

private:
    void init_move_controller()
    {
        auto &config = ConfigManager::instance().get_config();
        auto &mcontroller = get_movement_controller();
        mcontroller.set_mode(MovementController::MovementMode::InstantAccelSmoothStop);
        mcontroller.set_max_speed(config["physics"]["max_speed"].as_float());
        mcontroller.set_accel_rate(config["physics"]["accel_rate"].as_float());
        mcontroller.set_decel_rate(config["physics"]["decel_rate"].as_float());
    }
};

#endif // INCLUDE_PLAYER