#pragma once
#include <unordered_map>
#include <vector>
#include <algorithm>

// Event types for the application
enum class EventType {
    SettingsChanged,
    TimerStateChanged,
    WindowMoved,
    WindowResized,
    ThemeChanged,
    OpacityChanged,
    RenderRequired
};

// Event structure that carries type and optional data
struct Event {
    EventType type;
    void* data = nullptr;
};

// Interface for objects that want to listen to events
class IEventListener {
public:
    virtual ~IEventListener() = default;
    virtual void onEvent(const Event& event) = 0;
};

// Event manager using Observer pattern
class EventManager {
public:
    static EventManager& getInstance();

    void subscribe(EventType type, IEventListener* listener);
    void unsubscribe(EventType type, IEventListener* listener);
    void emit(const Event& event);

private:
    EventManager() = default;
    std::unordered_map<EventType, std::vector<IEventListener*>> listeners_;
};
