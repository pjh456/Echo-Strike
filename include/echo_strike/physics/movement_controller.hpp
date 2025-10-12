#ifndef INCLUDE_MOVEMENT_CONTROLLER
#define INCLUDE_MOVEMENT_CONTROLLER

#include <echo_strike/utils/vec2.hpp>

class MovementController
{
public:
    enum class MovementMode
    {
        Instant,
        Linear,
        LinearAccelInstantStop,
        InstantAccelSmoothStop
    };

private:
    MovementMode mode = MovementMode::Instant;
    float accel_rate = 50.0f;
    float decel_rate = 40.0f;
    float max_speed = 200.0f;

public:
    MovementController() = default;
    ~MovementController() = default;

public:
    Vec2 update_velocity(const Vec2 &cur, const Vec2 &dir, float ms)
    {
        float dt = ms / 1000.0f;
        auto new_vel = cur;

        Vec2 target = dir;
        if (target.length() > 1e-6f)
            target = target.normalize() * max_speed;
        else
            target = Vec2(0, 0);

        switch (mode)
        {
        case MovementMode::Instant:
            new_vel = target;
            break;
        case MovementMode::Linear:
            new_vel = move_towards(cur, target, ms);
            break;
        case MovementMode::LinearAccelInstantStop:
            if (dir.length() <= 1e-6)
                new_vel = Vec2(0, 0);
            else
                new_vel = move_towards(cur, target, ms);
            break;
        case MovementMode::InstantAccelSmoothStop:
            if (dir.length() > 1e-6)
                new_vel = target;
            else
                new_vel = move_towards(cur, {}, ms);
            break;
        }

        return new_vel;
    }

private:
    Vec2 move_towards(Vec2 cur, Vec2 target, float ms)
    {
        Vec2 diff = target - cur;
        if (diff.length() <= 1e-6)
            return cur;

        float rate = (target.length() > cur.length()) ? accel_rate : decel_rate;
        float change = rate * ms / 1000;
        if (diff.length() <= change)
            return target;

        return cur + diff.normalize() * change;
    }

public:
    MovementMode get_mode() const { return mode; }
    void set_mode(MovementMode m) { mode = m; }

    float get_accel_rate() const { return accel_rate; }
    void set_accel_rate(float v) { accel_rate = v; }

    float get_decel_rate() const { return decel_rate; }
    void set_decel_rate(float v) { decel_rate = v; }

    float get_max_speed() const { return max_speed; }
    void set_max_speed(float v) { max_speed = v; }
};
#endif // INCLUDE_MOVEMENT_CONTROLLER