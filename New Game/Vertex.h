#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>

struct Color {
public:
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
	Color() : r(0), g(0), b(0), a(0) {}
	Color(GLubyte R, GLubyte G, GLubyte B, GLubyte A) :
		r(R), g(G), b(B), a(A) {}
	Color(glm::vec4 col) :
		r(col[0]), g(col[1]), b(col[2]), a(col[3]) {}
	Color(glm::vec3 col) :
		r(col[0]), g(col[1]), b(col[2]), a(255) {}
};

struct Vertex {
	glm::vec2 pos;
	glm::vec2 uv;
	Color color;
	Vertex() : pos(0, 0), uv(0, 0), color(255, 255, 255, 255) {}
	Vertex(glm::vec2 p, glm::vec2 u, glm::vec4 c = { 255, 255, 255, 255 }) : pos(p), uv(u), color(c[0], c[1], c[2], c[3]) {}

	void setPosition(float x, float y) { pos.x = x; pos.y = y; }
	void setUV(float x, float y) { uv.x = x; uv.y = y; }
	void setColor(int r, int g, int b, int a = 255) { color.r = r; color.g = g; color.b = b; color.a = a; }
};

