#include "Time.h"

Time::Time() {
    lastFrameTime = std::chrono::steady_clock::now();
}

void Time::update() {
    lastFrameTime = std::chrono::steady_clock::now();
}

float Time::deltaTime() {
    auto currentTime = std::chrono::steady_clock::now();
    auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime).count() / 1000.0f;
    return deltaTime;
}
