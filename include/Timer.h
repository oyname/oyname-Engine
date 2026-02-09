#pragma once
#include <chrono>

class Timer {
public:
    enum class TimeMode {
        FIXED_TIMESTEP,
        VSYNC_ONLY
    };

private:
    TimeMode timeMode;
    double deltaTime;
    double accumulator;
    std::chrono::high_resolution_clock::time_point lastFrameTime;

    static constexpr double FIXED_TIMESTEP = 1.0 / 60.0;
    static constexpr double TARGET_FPS = 60.0;

    // Singleton
    static Timer* s_instance;

    Timer();

public:
    ~Timer();

    // Singleton-Zugriff
    static Timer* GetInstance();
    static void Initialize();
    static void Shutdown();

    // Frame-Time aktualisieren
    void Update();

    // Getter
    static double GetDeltaTime() { return GetInstance()->deltaTime; }
    static double GetAccumulator() { return GetInstance()->accumulator; }
    static TimeMode GetTimeMode() { return GetInstance()->timeMode; }
    static double GetFPS();

    // Setter
    static void SetTimeMode(TimeMode mode) { GetInstance()->timeMode = mode; }

    // Hilfsfunktionen
    static void Reset();
};