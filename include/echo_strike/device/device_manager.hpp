#ifndef INCLUDE_DEVICE_MANAGER
#define INCLUDE_DEVICE_MANAGER

#include <echo_strike/device/window.hpp>
#include <echo_strike/device/renderer.hpp>

class DeviceManager
{
public:
    static DeviceManager &instance();

private:
    Window *window = nullptr;
    Renderer *renderer = nullptr;

private:
    DeviceManager() = default;
    ~DeviceManager() = default;

    DeviceManager(const DeviceManager &) = delete;
    DeviceManager &operator=(const DeviceManager &) = delete;

    DeviceManager(DeviceManager &&) noexcept = delete;
    DeviceManager &operator=(DeviceManager &&) noexcept = delete;

public:
    Window *get_window() const { return window; }
    void set_window(Window *w) { window = w; }

    Renderer *get_renderer() const { return renderer; }
    void set_renderer(Renderer *r) { renderer = r; }
};

inline DeviceManager &DeviceManager::instance()
{
    static DeviceManager manager;
    return manager;
}

#endif // INCLUDE_DEVICE_MANAGER