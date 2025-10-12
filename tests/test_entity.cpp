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

int main()
{
    namespace fs = std::filesystem;
    fs::path config_path("E:/Projects/games/echo strike/config.json");
    fs::path resource_path("E:/Projects/games/echo strike/resources");
    Rect bound{0, 0, 800, 600};
    float speed_rate = 200.0;

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

    auto player_anims = config["animations"]["player"].get()->as_array()->as_vector();
    for (auto anim : player_anims)
    {
        auto obj = pjh_std::json::Ref(anim->as_object());
        auto [anim_key, anim_cache] = rmanager.load_atlas_cache(obj["key"].as_str());
        if (anim_cache)
        {
            auto anim_node = new EntityState(player);
            anim_node->get_anim().add_frames(*anim_cache);
            anim_node->get_anim().set_interval(obj["interval"].as_float());
            stm.register_state(obj["name"].as_str(), anim_node);
        }
    }

    stm.set_entry("idle");

    window.show();
    bool running = true;
    auto now = std::chrono::high_resolution_clock::now();

    bool w_event_handled, s_event_handled, a_event_handled, d_event_handled;
    w_event_handled = s_event_handled = a_event_handled = d_event_handled = false;

    while (running)
    {
        renderer.set_draw_color({0, 0, 0});
        renderer.clear();

        imanager.update();

        if (imanager.is_key_pressed(SDL_SCANCODE_ESCAPE))
            running = false;

        Vec2 move_direction(0.0f, 0.0f);
        if (imanager.is_key_pressed(SDL_SCANCODE_W))
            move_direction.set_y(move_direction.get_y() - 1.0f);
        if (imanager.is_key_pressed(SDL_SCANCODE_S))
            move_direction.set_y(move_direction.get_y() + 1.0f);
        if (imanager.is_key_pressed(SDL_SCANCODE_A))
            move_direction.set_x(move_direction.get_x() - 1.0f);
        if (imanager.is_key_pressed(SDL_SCANCODE_D))
            move_direction.set_x(move_direction.get_x() + 1.0f);

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

        auto cur_time = std::chrono::high_resolution_clock::now();
        auto dur_time = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - now);
        now = cur_time;

        emanager.on_update(dur_time.count());
        emanager.on_render(&renderer);
        player->get_hurt_box()->render_border(renderer.get_raw());

        renderer.present();

        std::cout << player->get_rect() << std::endl;

        SDL_Delay(16);
    }
}