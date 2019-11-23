#pragma once
#include <vector>

struct Animation {
	std::vector<unsigned int> frames;
	std::vector<float> frameTime;
	float timeSinceLastFrame = 0;
	unsigned int currentAnimation;
	bool active;

	Animation(unsigned int frames, float frameTime, bool active) :
		active(active),
		currentAnimation(0) {
		this->frames.push_back(frames);
		this->frameTime.push_back(frameTime);
	}
};