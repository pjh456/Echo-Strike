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
        Vec2 target = dir * max_speed;

        switch (mode)
        {
        case MovementMode::Instant:
            return target;
        case MovementMode::Linear:
            return move_towards(cur, target, ms);
        case MovementMode::LinearAccelInstantStop:
            if (dir.length() <= 1e-6)
                return Vec2();
            else
                return move_towards(cur, target, ms);
        case MovementMode::InstantAccelSmoothStop:
            if (dir.length() > 1e-6)
                return target;
            else
                return move_towards(cur, {}, ms);
        }
        return cur;
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