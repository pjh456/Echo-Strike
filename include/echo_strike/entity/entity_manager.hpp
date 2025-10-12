#ifndef INCLUDE_ENTITY_MANAGER
#define INCLUDE_ENTITY_MANAGER

#include <echo_strike/transform/rect.hpp>
#include <echo_strike/entity/entity.hpp>

#include <echo_strike/device/renderer.hpp>

#include <vector>

class Entity;

class EntityManager
{
public:
    static EntityManager &instance()
    {
        static EntityManager manager;
        return manager;
    }
    template <typename T>
    T *create_entity()
    {
        auto t = new T();
        if (auto ptr = static_cast<Entity *>(t))
        {
            ets.push_back(ptr);

            ptr->set_boundary(bound);
            ptr->set_motion_type(Object::MotionType::LimitedInBoundary);

            return t;
        }
        else
        {
            delete t;
            return nullptr;
        }
    }
    template <typename T>
    void destory_entity(T *entity)
    {
        if (auto ptr = static_cast<Entity *>(entity))
        {
            ets.erase(std::find(ets.begin(), ets.end(), ptr));
            delete entity;
        }
    }

private:
    Rect bound;
    std::vector<Entity *> ets;

private:
    EntityManager() = default;
    ~EntityManager() = default;

public:
    void on_update(float ms)
    {
        for (auto &e : ets)
            e->on_update(ms);
    }

    void on_render(Renderer *renderer)
    {
        for (auto &e : ets)
            e->on_render(renderer);
    }

public:
    Rect &boundary() { return bound; }
    const Rect &boundary() const { return bound; }

    std::vector<Entity *> &entities() { return ets; }
    const std::vector<Entity *> &entities() const { return ets; }
};

#endif // INCLUDE_ENTITY_MANAGER