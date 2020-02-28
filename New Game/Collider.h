#pragma once
#include <glm/glm.hpp>

struct Collider {
	bool circular;
	glm::vec2 dim;
	float radius;
	float mass;

	Collider(float radius) : circular(true), dim(0), radius(radius), mass(0) {}
	Collider(glm::vec2 dim) : circular(false), dim(dim), radius(-1), mass(0) {}
	Collider(float radius, float mass) : circular(true), dim(0), radius(radius), mass(mass) {}
};