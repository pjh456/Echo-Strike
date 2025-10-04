#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <cassert>

#include <echo_strike/physics/physical_object.hpp>
#include <echo_strike/physics/obstacle_object.hpp>
#include <echo_strike/collision/collision_manager.hpp>

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    auto window = SDL_CreateWindow("Collision Test", 800, 600, SDL_WINDOW_RESIZABLE);
    auto renderer = SDL_CreateRenderer(window, NULL);

    std::vector<PhysicaObject *> particles;
    std::vector<PhysicaObject *> del_particles;

    // 随机速度生成器
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> speed_dist(100.0f, 300.0f);

    // 创建墙壁（四个静态物体）
    std::vector<ObstacleObject *> walls;
    Rect boundary(0, 0, 800, 600);

    auto make_wall = [&](float x, float y, float w, float h)
    {
        auto *wall = new ObstacleObject();
        wall->set_rect(Rect(x, y, w, h));
        walls.push_back(wall);
        return wall;
    };

    make_wall(0, 0, 800, 10);   // top
    make_wall(0, 590, 800, 10); // bottom
    make_wall(390, 0, 10, 600); // center
    make_wall(0, 0, 10, 600);   // left
    make_wall(790, 0, 10, 600); // right

    bool running = true;
    SDL_Event event;
    Uint64 prev_ticks = SDL_GetTicks();

    while (running)
    {
        Uint64 current_ticks = SDL_GetTicks();
        float delta = (current_ticks - prev_ticks) / 1000.0f;
        prev_ticks = current_ticks;

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                // 点击后发射一个小方块
                float mouse_x = static_cast<float>(event.button.x);
                float mouse_y = static_cast<float>(event.button.y);

                Vec2 dir(mouse_x, mouse_y);
                dir = dir.normalize();

                float speed_val = speed_dist(rng);
                Vec2 velocity(dir.get_x() * speed_val, dir.get_y() * speed_val);

                auto *p = new PhysicaObject();
                p->set_rect(Rect(10, 10, 25, 25));
                p->set_speed(velocity);
                p->set_force(Vec2(0, 0));
                p->set_mess(1.0f);
                particles.push_back(p);

                assert(p->collision_box().get_src() == CollisionLayer::Physics);
                assert(p->collision_box().has_dst(CollisionLayer::Physics));
                assert(p->collision_box().has_dst(CollisionLayer::Obstacle));

                puts("create particle!");
            }
        }

        // 更新所有动态物体
        for (auto *p : particles)
        {
            if (p->get_rect().is_intersect(boundary))
            {
                p->on_update(delta);
            }
            else
                del_particles.push_back(p);
        }

        for (auto p : del_particles)
        {
            puts("destroy particle!");
            particles.erase(std::find(particles.begin(), particles.end(), p));
            delete p;
        }
        del_particles.clear();

        // 调用碰撞检测
        CollisionManager::instance().process_collide();

        // 绘制
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (auto *p : particles)
            p->render_full(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (auto *w : walls)
            w->render_full(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    // 清理
    for (auto *p : particles)
        delete p;
    for (auto *w : walls)
        delete w;

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
