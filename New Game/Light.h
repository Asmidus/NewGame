#pragma once
#include <glm/glm.hpp>



struct Light {
	Light(glm::vec3 col, float rad) : color(col, 255), radius(rad), pos(0.5f, 0.5f) {
		resolution = 512;
		auto [fbo, tex] = TextureManager::CreateRenderTexture(resolution, resolution);
		shadowFBO = fbo;
		shadowTex = tex;
	}
	Light(glm::vec2 center, glm::vec3 col, float rad) : color(col, 255), radius(rad), pos(center.x, center.y) {
		resolution = 512;
		auto [fbo, tex] = TextureManager::CreateRenderTexture(resolution, resolution);
		shadowFBO = fbo;
		shadowTex = tex;
	}
	//Light(glm::vec2 p, glm::vec3 col, float rad) : color(col, 255), radius(rad), pos(p) {}
	glm::vec2 pos;
	glm::vec4 color;
	float radius;
	unsigned int resolution;
	GLuint shadowFBO;
	GLuint shadowTex;
};