#include "Timing.h"
#include <iostream>
#include <SDL/SDL.h>
typedef std::chrono::high_resolution_clock Clock;

FPSLimiter::FPSLimiter(float maxFPS) { setMaxFPS(maxFPS); }

void FPSLimiter::setMaxFPS(float maxFPS) {
	_maxFPS = maxFPS;
}

void FPSLimiter::begin() {
	_start = Clock::now();
}

float FPSLimiter::end() {
	calculateFPS();
	while (1000000.0f / _maxFPS > std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - _start).count()) {}
	return _fps == 0 ? 0.001 : _fps;
}

void FPSLimiter::calculateFPS() {
	//static unsigned int fps = 0;
	//static std::chrono::steady_clock::time_point prevTicks = Clock::now();

	//auto currentTicks = Clock::now();
	//_frameTime = std::chrono::duration_cast<std::chrono::seconds>(currentTicks - prevTicks).count();
	//fps++;
	//if (_frameTime >= 1) {
	//	_fps = fps;
	//	fps = 0;
	//	prevTicks = currentTicks;
	//}

	static const int NUM_SAMPLES = 60;
	static float frameTimes[NUM_SAMPLES];
	static int currentFrame = 0;

	static std::chrono::steady_clock::time_point prevTicks = Clock::now();

	auto currentTicks = Clock::now();
	_frameTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTicks - prevTicks).count();
	//_frameTime = std::chrono::duration_cast<double, std::chrono::milliseconds>(currentTicks - prevTicks).count;
	frameTimes[currentFrame % NUM_SAMPLES] = _frameTime;
	prevTicks = currentTicks;
	int count;
	currentFrame++;
	if (currentFrame < NUM_SAMPLES) {
		count = currentFrame;
	} else {
		count = NUM_SAMPLES;
	}
	float frameTimeAverage = 0;
	for (int i = 0; i < NUM_SAMPLES; i++) {
		//std::cout << frameTimes[i] << "\n";
		frameTimeAverage += frameTimes[i];
	}
	frameTimeAverage /= count;
	if (frameTimeAverage > 0) {
		_fps = round(1000000.0f / frameTimeAverage);
	} else {
		_fps = 0.0f;
	}
}