#pragma once

// Configuration system for DBD-1v1-Timer
// Replaces all magic numbers with named constants

namespace Config {
    // Window defaults
    struct WindowDefaults {
        static constexpr int DEFAULT_WIDTH = 285;
        static constexpr int DEFAULT_HEIGHT = 40;
        static constexpr int MIN_WIDTH = 100;
        static constexpr int MIN_HEIGHT = 25;
        static constexpr int MAX_WIDTH = 1920;
        static constexpr int MAX_HEIGHT = 200;
        static constexpr int RESIZE_EDGE_THRESHOLD = 8;
    };

    // Rendering configuration
    struct RenderingConfig {
        static constexpr int TARGET_FPS = 60;
        static constexpr int TIMER_UPDATE_INTERVAL_MS = 100; // 10 FPS for timer updates
        static constexpr bool ENABLE_VSYNC = true;
    };

    // Default color indices (kept for backward compat with settings.json)
    struct DefaultColors {
        static constexpr int TIMER = 9;
        static constexpr int SELECTED_TIMER = 6;
        static constexpr int LAST_SECONDS = 1;
        static constexpr int BACKGROUND = 20;
    };

    // Opacity configuration
    struct OpacityConfig {
        static constexpr int MIN_OPACITY = 0;   // Fully transparent
        static constexpr int MAX_OPACITY = 255; // Fully opaque
        static constexpr int DEFAULT_OPACITY = 255;
        static constexpr int OPACITY_STEP = 15; // Amount to change per hotkey press
    };

    // Snapping configuration
    struct SnapConfig {
        static constexpr int SNAP_THRESHOLD = 20; // Pixels from edge to trigger snap
        static constexpr bool ENABLE_SNAP = true;
    };
}
