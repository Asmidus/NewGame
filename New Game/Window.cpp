#include "Window.h"
#include <GL/glew.h>



Window::Window() {}


Window::~Window() {}

void Window::create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags) {
	Uint32 flags = SDL_WINDOW_OPENGL;
	if (currentFlags & INVISIBLE) {
		flags |= SDL_WINDOW_HIDDEN;
	}
	if (currentFlags & FULLSCREEN) {
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if (currentFlags & BORDERLESS) {
		flags |= SDL_WINDOW_BORDERLESS;
	}
	_window = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, flags);
	if (_window == nullptr) throw;
	if (!SDL_GL_CreateContext(_window)) throw;
	if (glewInit() != GLEW_OK) throw;
	if (currentFlags & VSYNC) {
		SDL_GL_SetSwapInterval(1);
	} else {
		SDL_GL_SetSwapInterval(0);
	}
	std::printf("Running OpenGL Version: %s\n", glGetString(GL_VERSION));
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	// Enable alpha blend
	//glDisable(GL_CULL_FACE);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glDisable(GL_ALPHA_TEST);
}

void Window::swapBuffer() {
	 SDL_GL_SwapWindow(_window);
}
