#include <echo_strike/core/timer.hpp>

#include <utility>

Timer::Timer()
    : m_pass_time(0),
      m_timeout_time(0),
      m_paused(false),
      m_shotted(false),
      m_one_shot(false)
{
}

Timer::Timer(std::function<void()> &&callback)
    : Timer()
{
    timeout_callback = std::move(callback);
}

Timer::Timer(Timer &&other) noexcept
    : m_pass_time(other.m_pass_time),
      m_timeout_time(other.m_timeout_time),
      m_paused(other.m_paused),
      m_shotted(other.m_shotted),
      m_one_shot(other.m_one_shot),
      timeout_callback(std::move(other.timeout_callback))
{
}
Timer &Timer::operator=(Timer &&other) noexcept
{
    if (this == &other)
        return *this;

    m_pass_time = other.m_pass_time;
    m_timeout_time = other.m_timeout_time;
    m_paused = other.m_paused;
    m_shotted = other.m_shotted;
    m_one_shot = other.m_one_shot;
    timeout_callback = std::move(other.timeout_callback);
}

void Timer::restart() { m_pass_time = 0, m_paused = false; }

void Timer::pause() { m_paused = true; }

void Timer::resume() { m_paused = false; }

void Timer::update(float delta)
{
    if (m_paused)
        return;

    m_pass_time += delta;
    if (m_pass_time >= m_timeout_time)
    {
        bool can_shot = (!m_shotted) || (m_shotted && (!m_one_shot));
        m_shotted = true;

        if (can_shot && timeout_callback)
            timeout_callback();

        m_pass_time -= m_timeout_time;
    }
}