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

int main()
{
    namespace fs = std::filesystem;
    fs::path config_path("E:/Projects/games/echo strike/config.json");
    fs::path resource_path("E:/Projects/games/echo strike/resources");
    Rect bound{0, 0, 800, 600};
    float speed_rate = 200.0;
    float friction = 10.0;

    auto &imanager = InputManager::instance();
    auto window = Window::create().set_title("entity test").resizable(false).visible(false).size(bound.get_size()).build();
    auto renderer = window.create_renderer().build();

    auto &cmanager = ConfigManager::instance();
    cmanager.load_config(config_path);
    auto &rmanager = ResourceManager::instance();
    rmanager.load_atlases(renderer.get_raw(), resource_path, "{}.png");

    auto &emanager = EntityManager::instance();
    emanager.boundary() = bound;
    auto player = emanager.create_entity<Entity>();
    auto &stm = player->get_state_machine();

    auto [idle_key, idle_anim] = rmanager.load_atlas_cache("player/idle.png");
    auto [run_key, run_anim] = rmanager.load_atlas_cache("player/run.png");

    if (idle_anim)
    {
        auto idle_node = new EntityState(player);
        idle_node->get_anim().add_frames(*idle_anim);
        idle_node->get_anim().set_interval(100);
        stm.register_state("idle", idle_node);
    }
    if (run_anim)
    {
        auto run_node = new EntityState(player);
        run_node->get_anim().add_frames(*run_anim);
        run_node->get_anim().set_interval(50);
        stm.register_state("run", run_node);
    }

    stm.set_entry("idle");

    window.show();
    bool running = true;
    auto now = std::chrono::high_resolution_clock::now();

    bool w_event_handled, s_event_handled, a_event_handled, d_event_handled;
    w_event_handled = s_event_handled = a_event_handled = d_event_handled = false;

    while (running)
    {
        renderer.clear();
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

        if (move_direction.length() > 0)
        {
            stm.switch_to("run");
            player->set_speed(move_direction * speed_rate);
        }
        else
        {
            stm.switch_to("idle");
            player->set_speed({0.0f, 0.0f}), player->set_force({0.0f, 0.0f});
        }

        imanager.update();

        auto cur_time = std::chrono::high_resolution_clock::now();
        auto dur_time = std::chrono::duration_cast<std::chrono::milliseconds>(cur_time - now);
        now = cur_time;

        emanager.on_update(dur_time.count());
        emanager.on_render(&renderer);

        renderer.present();

        std::cout << player->get_rect() << std::endl;

        SDL_Delay(16);
    }
}