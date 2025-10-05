#ifndef INCLUDE_PHYSICS_MANAGER
#define INCLUDE_PHYSICS_MANAGER

#include <vector>

class PhysicalObject;
class CollisionBox;

struct SDL_Renderer;

class PhysicsManager
{
public:
    static PhysicsManager &instance();
    PhysicalObject *create_physical_object();
    void destroy_physical_object(PhysicalObject *);

    void on_update(float);
    void render(SDL_Renderer *);

private:
    std::vector<PhysicalObject *> objs;
    bool was_any_overlap_found = false;

private:
    PhysicsManager() = default;
    ~PhysicsManager();

public:
    size_t size() const { return objs.size(); }
    void clear();

    std::vector<PhysicalObject *> &objects() { return objs; }
    const std::vector<PhysicalObject *> &objects() const { return objs; }

public:
    void resolve_all_penetrations();

private:
    void handle_collision(CollisionBox &src, CollisionBox *dst);
};

#endif // INCLUDE_PHYSICS_MANAGER