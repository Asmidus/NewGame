#pragma once
#include "Transform.h"
#include "TextureManager.h"
#include "Vertex.h"

struct Sprite {
	Sprite() = default;

	Sprite(const char* path) {
		setSprite(path);
	}

	Sprite(const char* path, int w, int h, glm::vec3 color = { 255, 255, 255 }) {
		src.w = w;
		src.h = h;
		src.x = src.y = 0;
		this->color = color;
		setSprite(path);
	}

	void setSprite(const char* path) {
		auto [tex, w, h] = TextureManager::LoadTexture(path);
		texture = tex;
		size.x = w;
		size.y = h;
	}

	glm::vec4 getUV() {
		return glm::vec4(src.x / float(size.x), src.y / float(size.y), src.w / float(size.x), src.h / float(size.y));
	}

	SDL_Rect src, scrollDest;
	glm::ivec2 size;
	Color color;
	unsigned int texture;
};