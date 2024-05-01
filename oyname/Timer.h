#pragma once

#include <chrono>

class Timer {
public:
	Timer();

	void update();

	double deltaTime;
	double timeScale;

private:
	std::chrono::steady_clock::time_point lastTime;
};

