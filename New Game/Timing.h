#include <chrono>
class FPSLimiter {
public:
	FPSLimiter(float maxFPS);
	void setMaxFPS(float maxFPS);
	void begin();
	float end();		//returns current FPS

	void calculateFPS();
private:
	float _fps;
	float _maxFPS;
	float _frameTime;
	std::chrono::steady_clock::time_point _start;
};