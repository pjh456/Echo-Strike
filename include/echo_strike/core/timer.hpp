#ifndef INCLUDE_TIMER
#define INCLUDE_TIMER

#include <echo_strike/utils/class_marcos.hpp>

#include <functional>

class Timer
{
private:
    std::function<void()> timeout_callback;
    CLASS_READONLY_PROPERTY(float, pass_time)
    CLASS_PROPERTY(float, timeout_time)

    CLASS_PROPERTY(bool, paused)
    CLASS_PROPERTY(bool, shotted)
    CLASS_PROPERTY(bool, one_shot)

public:
    Timer();
    Timer(std::function<void()> &&);

    Timer(const Timer &) = delete;
    Timer &operator=(const Timer &) = delete;

    Timer(Timer &&) noexcept;
    Timer &operator=(Timer &&) noexcept;

public:
    void restart();
    void pause();
    void resume();
    void update(float);
};

#endif // INCLUDE_TIMER