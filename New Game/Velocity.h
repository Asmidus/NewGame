#pragma once
#include <glm/glm.hpp>

struct Velocity {
	Velocity(glm::vec2 dir, float acceleration, float max) :
		direction(dir),
		maxSpeed(max),
		accel(acceleration),
		decel(acceleration),
		currVel(0, 0),
		currAccel(acceleration),
		angular(0),
		constant(false) {}
	Velocity(glm::vec2 dir, float speed) :
		direction(dir),
		maxSpeed(speed),
		accel(1),
		decel(0),
		currAccel(0),
		currVel(glm::length(dir) ? glm::normalize(dir) * speed : glm::vec2(0, 0)),
		angular(0),
		constant(true) {}
	glm::vec2 direction;
	glm::vec2 currVel;
	float maxSpeed;
	float currAccel;
	float accel;
	float decel;
	float angular;
	bool constant;
};