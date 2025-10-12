#ifndef INCLUDE_EVENT
#define INCLUDE_EVENT

#include <string>
#include <utility>

class EventBuilder;

class Event
{
public:
    enum class EventType
    {
        KeyPressed,
        KeyReleased,
        KeyQuery,
        MousePressed,
        MouseReleased,
        MouseMove,
        Collision,
        None
    };

public:
    EventBuilder create();

protected:
    std::string name;
    EventType type;

public:
    Event(EventType t = EventType::None) : type(t) {}
    virtual ~Event() = default;

public:
    virtual std::string get_name() const { return name; }
    virtual void set_name(const std::string &key) { name = key; }

    virtual EventType get_type() const { return type; }
    virtual void set_type(EventType t) { type = t; }
};

class EventBuilder
{
public:
    Event build() { return event; }

private:
    Event event;

public:
    EventBuilder() = default;
    virtual ~EventBuilder() = default;

public:
    virtual void set_name(const std::string &key) { event.set_name(key); }
    virtual void set_type(Event::EventType t) { event.set_type(t); }
};

inline EventBuilder Event::create() { return {}; }

#endif // INCLUDE_EVENT