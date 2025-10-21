#include "EventManager.h"

EventManager& EventManager::getInstance() {
    static EventManager instance;
    return instance;
}

void EventManager::subscribe(EventType type, IEventListener* listener) {
    listeners_[type].push_back(listener);
}

void EventManager::unsubscribe(EventType type, IEventListener* listener) {
    auto& vec = listeners_[type];
    vec.erase(std::remove(vec.begin(), vec.end(), listener), vec.end());
}

void EventManager::emit(const Event& event) {
    if (listeners_.find(event.type) != listeners_.end()) {
        for (auto* listener : listeners_[event.type]) {
            listener->onEvent(event);
        }
    }
}
