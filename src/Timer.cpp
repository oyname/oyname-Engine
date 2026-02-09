#include "Timer.h"
#include <thread>
#include <algorithm>

Timer* Timer::s_instance = nullptr;

Timer::Timer()
    : timeMode(TimeMode::FIXED_TIMESTEP),
    deltaTime(0.0),
    accumulator(0.0),
    lastFrameTime(std::chrono::high_resolution_clock::now())
{
}

Timer::~Timer() {
}

Timer* Timer::GetInstance() {
    if (s_instance == nullptr) {
        s_instance = new Timer();
    }
    return s_instance;
}

void Timer::Initialize() {
    GetInstance();
}

void Timer::Shutdown() {
    if (s_instance != nullptr) {
        delete s_instance;
        s_instance = nullptr;
    }
}

void Timer::Update() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    double rawDeltaTime = std::chrono::duration<double>(currentTime - lastFrameTime).count();
    lastFrameTime = currentTime;

    // Cap gegen massive Stutters
    rawDeltaTime = std::min(rawDeltaTime, 0.1);

    if (timeMode == TimeMode::FIXED_TIMESTEP) {
        accumulator += rawDeltaTime;

        if (accumulator >= FIXED_TIMESTEP) {
            deltaTime = FIXED_TIMESTEP;
            accumulator -= FIXED_TIMESTEP;
        }
        else {
            deltaTime = 0.0;
        }

        // Frame-Rate-Limiting
        double targetFrameTime = 1.0 / TARGET_FPS;
        if (rawDeltaTime < targetFrameTime) {
            std::this_thread::sleep_for(
                std::chrono::duration<double>(targetFrameTime - rawDeltaTime)
            );
        }
    }
    else if (timeMode == TimeMode::VSYNC_ONLY) {
        deltaTime = rawDeltaTime;
    }
}

void Timer::Reset() {
    GetInstance()->deltaTime = 0.0;
    GetInstance()->accumulator = 0.0;
    GetInstance()->lastFrameTime = std::chrono::high_resolution_clock::now();
}

double Timer::GetFPS() {
    if (GetInstance()->deltaTime > 0.0) {
        return 1.0 / GetInstance()->deltaTime;
    }
    return TARGET_FPS;
}
