#include <SDL3/SDL.h>

#include <chrono>
#include <filesystem>
#include <iostream>

#include <echo_strike/device/device_manager.hpp>
#include <echo_strike/device/camera2d.hpp>
#include <echo_strike/device/input_manager.hpp>
#include <echo_strike/device/window.hpp>
#include <echo_strike/image/atlas.hpp>
#include <echo_strike/image/animation.hpp>
#include <echo_strike/config/resource_manager.hpp>
#include <echo_strike/config/config_manager.hpp>

int main()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // -------------------------
    // 创建窗口和渲染器
    // -------------------------
    auto window =
        Window::create()
            .set_title("Camera2D + Input Demo")
            .resizable(true)
            .size(800, 600)
            .build();

    auto renderer = window.create_renderer().build();

    // -------------------------
    // 设置 DeviceManager
    // -------------------------
    auto &device_manager = DeviceManager::instance();
    device_manager.set_window(&window);
    device_manager.set_renderer(&renderer);

    auto &input = InputManager::instance();

    // -------------------------
    // 设置 Camera2D
    // -------------------------
    Rect viewport(0, 0, 800, 600);
    Camera2D camera(viewport);
    camera.set_zoom(1.0f);
    camera.set_origin({400, 300}); // 屏幕中心
    renderer.set_camera(&camera);

    // -------------------------
    // 加载资源
    // -------------------------
    auto config_dir = std::filesystem::path("E:/Projects/games/echo strike/config.json");
    ConfigManager::instance().load_config(config_dir);
    auto &res_manager = ResourceManager::instance();
    auto res_dir = std::filesystem::path("E:/Projects/games/echo strike/resources");
    res_manager.set_resource_folder(res_dir);
    auto atlases = res_manager.load_atlases(
        renderer.get_raw(),
        res_dir,
        "{}.png");

    // 背景
    auto [bg_key, bg_atlas] = res_manager.load_atlas_cache("background.png");
    auto &bg_image = (*bg_atlas)[0];

    // 玩家动画
    auto [atlas_key, atlas] = res_manager.load_atlas_cache("player/run.png");
    Animation player_anim(50);
    player_anim.add_frames(*atlas);
    Vec2 player_pos(0, 0); // 世界坐标中心

    // -------------------------
    // 时钟
    // -------------------------
    auto last_time = std::chrono::high_resolution_clock::now();

    bool running = true;
    while (running)
    {
        input.update();

        // -------------------------
        // 检查退出
        // -------------------------
        if (input.is_key_pressed(SDL_SCANCODE_ESCAPE))
            running = false;

        // -------------------------
        // 玩家移动 (WASD)
        // -------------------------
        float speed = 200.0f; // pixels per second
        auto dt = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - last_time).count();
        last_time = std::chrono::high_resolution_clock::now();

        Vec2 move_delta(0, 0);
        if (input.is_key_pressed(SDL_SCANCODE_W))
            move_delta += Vec2(0, -1);
        if (input.is_key_pressed(SDL_SCANCODE_S))
            move_delta += Vec2(0, 1);
        if (input.is_key_pressed(SDL_SCANCODE_A))
            move_delta += Vec2(-1, 0);
        if (input.is_key_pressed(SDL_SCANCODE_D))
            move_delta += Vec2(1, 0);
        player_pos += move_delta * speed * dt;
        player_anim.set_position(player_pos);

        // -------------------------
        // 摄像机移动 (箭头)
        // -------------------------
        Vec2 cam_delta(0, 0);
        if (input.is_key_pressed(SDL_SCANCODE_UP))
            cam_delta += Vec2(0, -1);
        if (input.is_key_pressed(SDL_SCANCODE_DOWN))
            cam_delta += Vec2(0, 1);
        if (input.is_key_pressed(SDL_SCANCODE_LEFT))
            cam_delta += Vec2(-1, 0);
        if (input.is_key_pressed(SDL_SCANCODE_RIGHT))
            cam_delta += Vec2(1, 0);
        camera.move(cam_delta * speed * dt);

        // -------------------------
        // 渲染
        // -------------------------
        renderer.set_draw_color({30, 30, 30});
        renderer.clear();

        if (bg_atlas)
        {
            int bg_w = bg_image.get_width();
            int bg_h = bg_image.get_height();

            // 1. 获取相机当前在世界坐标系中的可视区域
            Rect camera_view = camera.get_viewport();

            // 2. 计算需要绘制的背景图块的索引范围
            //    使用 floor 和 ceil 来确保覆盖整个可视区域，即使相机位置为小数或负数
            int start_x_idx = static_cast<int>(std::floor(camera_view.left() / bg_w));
            int end_x_idx = static_cast<int>(std::ceil(camera_view.right() / bg_w));

            // 根据您的 Rect 实现 (bottom-left origin, Y-up), 使用 bottom 和 top
            int start_y_idx = static_cast<int>(std::floor(camera_view.bottom() / bg_h));
            int end_y_idx = static_cast<int>(std::ceil(camera_view.top() / bg_h));

            // 3. 遍历这些索引，计算每个图块的世界坐标并绘制
            for (int i = start_x_idx - 1; i < end_x_idx + 1; ++i)
            {
                for (int j = start_y_idx - 1; j < end_y_idx + 1; ++j)
                {
                    float x_pos = static_cast<float>(i * bg_w);
                    float y_pos = static_cast<float>(j * bg_h);

                    // 创建一个代表单个背景图块的世界坐标矩形
                    Rect world_tile_rect(x_pos, y_pos, static_cast<float>(bg_w), static_cast<float>(bg_h));

                    // 将这个世界坐标的矩形交给渲染器
                    // 渲染器内部的 draw_texture 会自动通过相机把它转换到正确的屏幕位置
                    renderer.draw_texture(bg_image.get_texture(), &world_tile_rect);
                }
            }
        }

        // 玩家动画
        player_anim.update(int(dt * 1000));
        player_anim.render(&renderer);

        renderer.present();
        SDL_Delay(16); // ~60fps
    }

    return 0;
}
