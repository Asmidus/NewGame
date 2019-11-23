#pragma once
#include "SDL_ttf.h"
#include <iostream>

struct Text {
	TTF_Font* font;
	const char* fontFile = "media/font.ttf";
	const char* text;
	SDL_Texture* texture;
	SDL_Rect dest;
	glm::vec2 offset;
	SDL_Color color;
	float size;

	Text(const char* text, float size) : text(text), size(size), color({ 255, 255, 255, 255 }), font(nullptr), texture(nullptr), offset(0, 0) {
		dest.x = dest.y = dest.w = dest.h = 0;
		init();
	}

	Text(const char* text, float size, SDL_Color color) : text(text), size(size), color(color), font(nullptr), texture(nullptr), offset(0, 0) {
		dest.x = dest.y = dest.w = dest.h = 0;
		init();
	}

	Text(const char* text, float w, float h, float size) : text(text), size(size), color({ 255, 255, 255, 255 }), font(nullptr), texture(nullptr) {
		init();
		while (w * 14 / 15.0 < dest.w) {
			size -= 5;
			init();
		}
		offset.x = w / 2.0 - dest.w / 2.0;
		offset.y = h / 2.0 - dest.h / 2.0;
	}

	Text(const char* text, float w, float h, float size, SDL_Color color) : text(text), size(size), color(color), font(nullptr), texture(nullptr) {
		init();
		while (w * 14 / 15.0 < dest.w) {
			size -= 15;
			init();
		}
		offset.x = w / 2.0 - dest.w / 2.0;
		offset.y = h / 2.0 - dest.h / 2.0;
	}

	void init() {
		font = TTF_OpenFont(fontFile, size);
		texture = TextureManager::LoadText(text, font, color);
		SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);
		TTF_SetFontHinting(font, TTF_HINTING_LIGHT);
	}
};