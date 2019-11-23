#pragma once
#include <GL/glew.h>



class Program {
public:
	Program();
	~Program();

	void compileShaders(const char* vertexFile, const char* fragFile);
	void compileShader(const char* fileName, GLuint id);
	void link();
	void addAttribute(const char* name);
	GLint getUniformLocation(const char* name);

	void use();
	void unuse();
private:
	unsigned int _attributeCount;

	GLuint _id;
	GLuint _vertex;
	GLuint _frag;
};

