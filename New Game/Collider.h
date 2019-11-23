#pragma once
#include <glm/glm.hpp>

struct Collider {
	bool circular;
	glm::vec2 dim;
	float radius;

	Collider(float radius) : circular(true), dim(0), radius(radius) {}
	Collider(glm::vec2 dim) : circular(false), dim(dim), radius(-1) {}
};