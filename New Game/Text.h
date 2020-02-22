#pragma once
#include "SDL_ttf.h"
#include <iostream>

struct Text {
	TTF_Font* font;
	const char* fontFile = "media/font.ttf";
	const char* text;
	GLuint texture;
	SDL_Rect dest;
	SDL_Rect src;
	glm::vec2 offset;
	Color color;
	float size;

	Text(const char* text, float size) : text(text), size(size), color(glm::vec4{ 255, 255, 255, 255 }), font(nullptr), texture(0), offset(0, 0) {
		dest.x = dest.y = dest.w = dest.h = 0;
		init();
	}

	Text(const char* text, float size, Color color) : text(text), size(size), color(color), font(nullptr), texture(0), offset(0, 0) {
		dest.x = dest.y = dest.w = dest.h = 0;
		init();
	}

	Text(const char* text, float w, float h, float size) : text(text), size(size), color(glm::vec4{ 255, 255, 255, 255 }), font(nullptr), texture(0) {
		init();
		while (w * 14 / 15.0 < dest.w) {
			size -= 5;
			init();
		}
		offset.x = w / 2.0 - dest.w / 2.0;
		offset.y = h / 2.0 - dest.h / 2.0;
	}

	Text(const char* text, float w, float h, float size, Color color) : text(text), size(size), color(color), font(nullptr), texture(0) {
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
		SDL_Color sdlColor;
		sdlColor.r = color.r;
		sdlColor.g = color.g;
		sdlColor.b = color.b;
		sdlColor.a = color.a;
		texture = TextureManager::LoadText(text, font, sdlColor);
		glBindTexture(GL_TEXTURE_2D, texture);
		int miplevel = 0;
		glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &dest.w);
		glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &dest.h);
		src.w = dest.w;
		src.h = dest.h;
		src.x = src.y = 0;
		glBindTexture(GL_TEXTURE_2D, 0);
		//SDL_QueryTexture(texture, nullptr, nullptr, &dest.w, &dest.h);
		TTF_SetFontHinting(font, TTF_HINTING_LIGHT);
	}
};