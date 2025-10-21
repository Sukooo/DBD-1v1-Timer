#pragma once
#include "EventManager.h"
#include <functional>
#include <chrono>

// Manages rendering with dirty flag system and FPS throttling
class RenderManager : public IEventListener {
public:
    static RenderManager& getInstance();

    void markDirty();
    void render();
    bool isDirty() const { return isDirty_; }
    bool shouldRender();  // Checks dirty flag + FPS throttle

    void onEvent(const Event& event) override;

    void setRenderCallback(std::function<void()> callback);

private:
    RenderManager();
    bool isDirty_ = true;
    std::function<void()> renderCallback_;
    std::chrono::steady_clock::time_point lastRenderTime_;

    void subscribeToEvents();
};
