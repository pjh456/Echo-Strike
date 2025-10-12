#ifndef INCLUDE_EVENT_BUS
#define INCLUDE_EVENT_BUS

#include <echo_strike/event/event.hpp>

#include <utility>
#include <functional>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <typeindex>
#include <memory>

class EventBus
{
public:
    using KeyType = std::type_index;
    using Callback = std::function<void(const std::shared_ptr<Event> &)>;
    using CallbackList = std::vector<Callback>;
    using ListenerTable = std::unordered_map<KeyType, CallbackList>;

private:
    ListenerTable listeners;

public:
    static EventBus &instance()
    {
        static EventBus bus;
        return bus;
    }

private:
    EventBus() = default;
    ~EventBus() = default;

    EventBus(const EventBus &) = delete;
    EventBus &operator=(const EventBus &) = delete;

    EventBus(EventBus &&) noexcept = delete;
    EventBus &operator=(EventBus &&) noexcept = delete;

public:
    template <
        typename T,
        typename Func,
        std::enable_if_t<
            std::is_base_of_v<Event, T> &&
                std::is_invocable_v<Func, T *>,
            int> = 0>
    void subscribe(Func &&func)
    {
        listeners[typeid(T)].push_back(
            [func](const std::shared_ptr<Event> &event)
            { func(static_cast<T *>(event.get())); });
    }

    template <typename T, std::enable_if_t<std::is_base_of_v<Event, T>, int> = 0>
    void publish(const std::shared_ptr<T> &event)
    {
        auto it = listeners.find(typeid(T));
        if (it == listeners.end())
            return;

        for (auto &listener : it->second)
            listener(event);
    }
};

#endif // INCLUDE_EVENT_BUS