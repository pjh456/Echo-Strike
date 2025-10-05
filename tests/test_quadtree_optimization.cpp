#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>

#include <echo_strike/physics/physical_object.hpp>
#include <echo_strike/physics/physics_manager.hpp>
#include <echo_strike/physics/obstacle_object.hpp>
#include <echo_strike/collision/collision_manager.hpp>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    auto window = SDL_CreateWindow("Particle Test", 800, 600, SDL_WINDOW_RESIZABLE);
    auto renderer = SDL_CreateRenderer(window, NULL);

    std::vector<PhysicalObject *> particles;
    std::vector<PhysicalObject *> del_particles;

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> speed_dist(100.0f, 300.0f);

    std::vector<ObstacleObject *> walls;
    Rect boundary(0, 0, 800, 600);
    Vec2 gravity(0, 1000);

    auto make_wall = [&](float x, float y, float w, float h)
    {
        auto *wall = new ObstacleObject();
        wall->set_rect(Rect(x, y, w, h));
        walls.push_back(wall);
        return wall;
    };

    make_wall(0, 0, 800, 10);   // top
    make_wall(0, 590, 800, 10); // bottom
    make_wall(0, 0, 10, 600);   // left
    make_wall(790, 0, 10, 600); // right

    bool running = true;
    SDL_Event event;
    Uint64 prev_ticks = SDL_GetTicks();

    bool mouse_down = false;
    int mouse_x = 0, mouse_y = 0;

    // 生成速率控制
    const float spawn_rate = 30.0f; // 每秒最大生成 30 粒子
    float spawn_accumulator = 0.0f;

    // FPS统计
    int frame_count = 0;
    float fps_timer = 0.0f;

    while (running)
    {
        Uint64 current_ticks = SDL_GetTicks();
        float delta = (current_ticks - prev_ticks) / 1000.0f;
        prev_ticks = current_ticks;

        fps_timer += delta;
        frame_count++;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                mouse_down = true;
                mouse_x = event.button.x;
                mouse_y = event.button.y;
            }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP && event.button.button == SDL_BUTTON_LEFT)
            {
                mouse_down = false;
            }
            else if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
            }
        }

        // 粒子生成逻辑（按住鼠标 + 速率限制）
        if (mouse_down)
        {
            spawn_accumulator += delta * spawn_rate;
            while (spawn_accumulator >= 1.0f)
            {
                spawn_accumulator -= 1.0f;

                Vec2 dir(0.0f, -1.0f); // 粒子向上
                float speed_val = speed_dist(rng);
                Vec2 velocity(dir.get_x() * speed_val, dir.get_y() * speed_val);

                auto *p = PhysicsManager::instance().create_physical_object();
                p->set_rect(Rect(mouse_x, mouse_y, 15, 15));
                p->set_speed(velocity);
                p->set_force(gravity);
                p->set_mess(1.0f);

                assert(p->collision_box().get_src() == CollisionLayer::Physics);
                assert(p->collision_box().has_dst(CollisionLayer::Physics));
                assert(p->collision_box().has_dst(CollisionLayer::Obstacle));
            }
        }

        // 更新粒子
        PhysicsManager::instance().on_update(delta);

        // 删除越界粒子
        for (auto *p : PhysicsManager::instance().objects())
        {
            if (!(p->get_rect().is_intersect(boundary)))
                PhysicsManager::instance().destroy_physical_object(p);
        }

        // 渲染
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        PhysicsManager::instance().render(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (auto *w : walls)
            w->render_full(renderer);

        SDL_RenderPresent(renderer);

        // FPS输出
        if (fps_timer >= 1.0f)
        {
            std::cout << "FPS: " << frame_count << std::endl;
            frame_count = 0;
            fps_timer = 0.0f;
        }

        SDL_Delay(16); // ~60fps
    }

    for (auto *p : particles)
        delete p;
    for (auto *w : walls)
        delete w;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
