#pragma once
#include <glm/glm.hpp>

struct AsteroidSpawner {
	int numAsteroids;
	int increment;
	glm::vec2 speedRange;
	glm::vec2 sizeRange;
};