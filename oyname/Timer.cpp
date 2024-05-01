#include "Timer.h"

Timer::Timer():lastTime(std::chrono::steady_clock::now()), deltaTime(0.0), timeScale(1.0) {}

void Timer::update() {
    auto currentTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - lastTime);
    deltaTime = duration.count() * timeScale; // Berechnung mit timeScale
    lastTime = currentTime;
}

