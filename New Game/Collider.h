#pragma once
#include <glm/glm.hpp>

struct Collider {
	bool circular;
	glm::vec2 dim;
	float radius;
	float mass;

	Collider(float radius) : circular(true), dim(0), radius(radius), mass(FLT_EPSILON) {}
	Collider(glm::vec2 dim) : circular(false), dim(dim), radius(-1), mass(FLT_EPSILON) {}
	Collider(float radius, float mass) : circular(true), dim(0), radius(radius), mass(mass) {
		if (this->mass == 0) {
			this->mass = FLT_EPSILON;
		}
	}
};