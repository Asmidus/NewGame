#pragma once
#include <SDL.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <SDL_ttf.h>
#include <GL/glew.h>



namespace TextureManager {
	std::tuple<unsigned int, unsigned int, unsigned int> LoadTexture(const char* fileName);
	std::pair<GLuint, GLuint> CreateRenderTexture(int width, int height, bool smooth = false);
	std::vector<std::pair<GLuint, GLuint>> GetRenderTextures(unsigned int size);
	void DrawTexture(GLuint texture);
	void ClearTexture(GLuint fbo);
	GLuint LoadText(const char* text, TTF_Font* font, SDL_Color color);
}