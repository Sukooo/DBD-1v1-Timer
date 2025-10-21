#include "RenderManager.h"
#include "Configuration.h"
#include <algorithm>

RenderManager& RenderManager::getInstance() {
    static RenderManager instance;
    return instance;
}

RenderManager::RenderManager() {
    subscribeToEvents();
    lastRenderTime_ = std::chrono::steady_clock::now();
}

void RenderManager::subscribeToEvents() {
    auto& em = EventManager::getInstance();
    em.subscribe(EventType::TimerStateChanged, this);
    em.subscribe(EventType::SettingsChanged, this);
    em.subscribe(EventType::WindowResized, this);
    em.subscribe(EventType::ThemeChanged, this);
    em.subscribe(EventType::OpacityChanged, this);
}

void RenderManager::markDirty() {
    isDirty_ = true;
}

bool RenderManager::shouldRender() {
    if (!isDirty_) return false;

    // FPS throttling
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRenderTime_);

    constexpr int minFrameTime = 1000 / Config::RenderingConfig::TARGET_FPS;
    return elapsed.count() >= minFrameTime;
}

void RenderManager::render() {
    if (renderCallback_) {
        renderCallback_();
    }
    isDirty_ = false;
    lastRenderTime_ = std::chrono::steady_clock::now();
}

void RenderManager::onEvent(const Event& event) {
    markDirty();
}

void RenderManager::setRenderCallback(std::function<void()> callback) {
    renderCallback_ = callback;
}
