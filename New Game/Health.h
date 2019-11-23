#pragma once

struct Health {
	float current;
	float max;

	Health(float health) : current(health), max(health) {}
};