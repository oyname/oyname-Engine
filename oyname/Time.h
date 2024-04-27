#pragma once

#include <chrono>

class Time {
private:
    std::chrono::steady_clock::time_point lastFrameTime;

public:
    Time();
    void update();
    float deltaTime();
};

