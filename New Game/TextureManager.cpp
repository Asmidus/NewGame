#include "TextureManager.h"
#include "lodepng.h"
#include "Vertex.h"
#include <iostream>



static std::unordered_map<const char*, std::tuple<unsigned int, unsigned int, unsigned int>> _textures;
static std::unordered_map<unsigned int, std::vector<std::pair<GLuint, GLuint>>> _renderTextures;

std::tuple<unsigned int, unsigned int, unsigned int> TextureManager::LoadTexture(const char* fileName) {
	if (_textures.find(fileName) == _textures.end()) {
		std::vector<unsigned char> image;
		unsigned width, height;
		if (lodepng::decode(image, width, height, fileName)) {
			std::cout << "Texture " << fileName << " failed to load\n";
		}
		GLuint texture;
		//Generate the openGL texture object
		glGenTextures(1, &(texture));

		//Bind the texture object
		glBindTexture(GL_TEXTURE_2D, texture);
		//Upload the pixels to the texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &(image[0]));

		//Set some texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//Generate the mip maps
		glGenerateMipmap(GL_TEXTURE_2D);

		//Unbind the texture
		glBindTexture(GL_TEXTURE_2D, 0);

		_textures[fileName] = std::make_tuple(texture, width, height);
	}
	return _textures[fileName];
}

std::pair<GLuint, GLuint> TextureManager::CreateRenderTexture(int width, int height, bool smooth) {
	GLuint texture;
	//Generate the openGL texture object
	glGenTextures(1, &(texture));

	//Bind the texture object
	glBindTexture(GL_TEXTURE_2D, texture);
	//Upload the pixels to the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	//Set some texture parameters
	if (smooth) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	//Unbind the texture
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return std::make_pair(fbo, texture);
}


std::vector<std::pair<GLuint, GLuint>> TextureManager::GetRenderTextures(unsigned int size) {
	if (_renderTextures.find(size) == _renderTextures.end()) {
		_renderTextures[size] = { CreateRenderTexture(size, size), CreateRenderTexture(size, size), CreateRenderTexture(2, size), CreateRenderTexture(size, size) };
	}
	return _renderTextures[size];
}
void TextureManager::DrawTexture(GLuint texture) {
	glBindTexture(GL_TEXTURE_2D, texture);
	//Generate the buffer if it hasn't already been generated
	GLuint _vboID = 0;
	glGenBuffers(1, &_vboID);
	unsigned int x = 0;
	unsigned int y = 0;
	unsigned int width = 500;
	unsigned int height = 500;

	//This array will hold our vertex data.
	//We need 6 vertices, and each vertex has 2
	//floats for X and Y
	Vertex vertexData[6];

	//First Triangle
	vertexData[0].setPosition(x + width, y + height);
	vertexData[0].setUV(1.0f, 1.0f);

	vertexData[1].setPosition(x, y + height);
	vertexData[1].setUV(0.0f, 1.0f);

	vertexData[2].setPosition(x, y);
	vertexData[2].setUV(0.0f, 0.0f);

	//Second Triangle
	vertexData[3].setPosition(x, y);
	vertexData[3].setUV(0.0f, 0.0f);

	vertexData[4].setPosition(x + width, y);
	vertexData[4].setUV(1.0f, 0.0f);

	vertexData[5].setPosition(x + width, y + height);
	vertexData[5].setUV(1.0f, 1.0f);

	//Set all vertex colors to magenta
	for (int i = 0; i < 6; i++) {
		vertexData[i].setColor(255, 255, 255, 255);
	}


	//Tell opengl to bind our vertex buffer object
	glBindBuffer(GL_ARRAY_BUFFER, _vboID);
	//Upload the data to the GPU
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	//Tell opengl what attribute arrays we need
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//This is the position attribute pointer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	//This is the color attribute pointer
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	//This is the UV attribute pointer
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	//Draw the 6 vertices to the screen
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//Disable the vertex attrib arrays. This is not optional.
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	//Unbind the buffer (optional)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &_vboID);
}

void TextureManager::ClearTexture(GLuint fbo) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint TextureManager::LoadText(const char* text, TTF_Font* font, SDL_Color color) {
	GLuint id = 0;
	auto texSurface = TTF_RenderText_Blended(font, text, color);
	//auto tex = SDL_CreateTextureFromSurface(_renderer, texSurface);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	//int Mode = GL_RGB;

	//if (texSurface->format->BytesPerPixel == 4) {
	//	Mode = GL_RGBA8;
	//}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSurface->w, texSurface->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, texSurface->pixels);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Set some texture parameters
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(texSurface);
	return id;
}