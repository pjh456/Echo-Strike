#include <iostream>
#include <SDL3/SDL.h>
#include <filesystem>

#include <echo_strike/entity/entity_state.hpp>
#include <echo_strike/entity/entity_manager.hpp>

#include <echo_strike/device/camera2d.hpp>
#include <echo_strike/device/window.hpp>
#include <echo_strike/device/renderer.hpp>
#include <echo_strike/device/input_manager.hpp>

#include <echo_strike/config/config_manager.hpp>
#include <echo_strike/config/resource_manager.hpp>

#include <echo_strike/collision/collision_box.hpp>

#include <echo_strike/event/events.hpp>
#include <echo_strike/event/event_bus.hpp>

int main()
{
    namespace fs = std::filesystem;
    fs::path config_path("E:/Projects/games/echo strike/config.json");
    fs::path resource_path("E:/Projects/games/echo strike/resources");
    Rect bound{0, 0, 800, 600};
    Vec2 move_direction(0.0f, 0.0f);

    auto &imanager = InputManager::instance();
    auto window =
        Window::create()
            .set_title("entity test")
            .resizable(false)
            .visible(false)
            .size(bound.get_size())
            .build();
    auto renderer =
        window
            .create_renderer()
            .build();

    auto &cmanager = ConfigManager::instance();
    cmanager.load_config(config_path);
    auto &config = cmanager.get_config();
    auto &rmanager = ResourceManager::instance();
    rmanager.load_atlases(renderer.get_raw(), resource_path, "{}.png");

    auto &emanager = EntityManager::instance();
    emanager.boundary() = bound;
    auto player = emanager.create_entity<Entity>();

    auto &mcontroller = player->get_movement_controller();
    mcontroller.set_mode(MovementController::MovementMode::InstantAccelSmoothStop);
    mcontroller.set_max_speed(config["physics"]["max_speed"].as_float());
    mcontroller.set_accel_rate(config["physics"]["accel_rate"].as_float());
    mcontroller.set_decel_rate(config["physics"]["decel_rate"].as_float());

    auto &stm = player->get_state_machine();
    player->load_animations("player");
    stm.set_entry("idle");

    auto &bus = EventBus::instance();

    window.show();
    bool esc_pressed = false, w_pressed, s_pressed, a_pressed, d_pressed;
    auto now = std::chrono::high_resolution_clock::now();

    bus.subscribe<MouseMoveEvent>(
        [](MouseMoveEvent *event)
        {
            std::cout
                << event->get_world_pos()
                << ", "
                << event->get_delta_pos()
                << std::endl;
        });

    while (!esc_pressed)
    {
        renderer.set_draw_color({0, 0, 0});
        renderer.clear();

        imanager.update();

        bus.publish(std::make_shared<KeyQueryEvent>(SDL_SCANCODE_W, w_pressed));
        bus.publish(std::make_shared<KeyQueryEvent>(SDL_SCANCODE_S, s_pressed));
        bus.publish(std::make_shared<KeyQueryEvent>(SDL_SCANCODE_A, a_pressed));
        bus.publish(std::make_shared<KeyQueryEvent>(SDL_SCANCODE_D, d_pressed));
        bus.publish(std::make_shared<KeyQueryEvent>(SDL_SCANCODE_ESCAPE, esc_pressed));

        if (w_pressed)
            move_direction += Vec2(0, -1.0f);
        if (s_pressed)
            move_direction += Vec2(0, 1.0f);
        if (a_pressed)
            move_direction += Vec2(-1.0f, 0);
        if (d_pressed)
            move_direction += Vec2(1.0f, 0);

        if (move_direction.length() > 1e-6)
        {
            stm.switch_to("run");
            player->set_move_direction(move_direction);
        }
        else
        {
            stm.switch_to("idle");
            player->set_move_direction(move_direction);
        }

        move_direction = Vec2(0.0f, 0.0f);

        auto cur_time = std::chrono::high_resolution_clock::now();
        auto dur_time = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - now);
        now = cur_time;

        emanager.on_update(dur_time.count());
        emanager.on_render(&renderer);
        player->get_hurt_box()->render_border(renderer.get_raw());

        renderer.present();

        // std::cout << player->get_rect() << std::endl;

        SDL_Delay(16);
    }
}