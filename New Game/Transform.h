#pragma once
#include <SDL.h>
#include <glm/glm.hpp>

struct Transform {
	//angle of rotation
	float angle;
	//the position and dimensions
	SDL_FRect rect;
	//z level the transform exists on, 0 is the top most layer
	unsigned int z;
	//center is multiplicative so 0.5, 0.5 is 50% of the width from the left and 50% of the height from the top
	glm::vec2 center;

	Transform() {
		center.x = 25;
		center.y = 25;
		rect.x = 0;
		rect.y = 0;
		rect.w = 50;
		rect.h = 50;
		angle = 0.0f;
		z = 0;
	}

	Transform(float x, float y) : Transform() {
		rect.x = x;
		rect.y = y;
		center.x = 0.5;
		center.y = 0.5;
	}
	
	Transform(float x, float y, float w, float h, unsigned int zLevel) : Transform(x, y) {
		rect.w = w;
		rect.h = h;
		z = zLevel;
	}

	Transform(float x, float y, float w, float h, unsigned int zLevel, float cx, float cy) : Transform(x, y, w, h, zLevel) {
		center.x = cx;
		center.y = cy;
	}

	Transform(const Transform& other, float w, float h) : Transform(other.rect.x, other.rect.y, w, h, other.z) {
		angle = other.angle;
	}

	void updatePos(const glm::vec2& delta) {
		rect.x += delta.x;
		rect.y += delta.y;
	}
};