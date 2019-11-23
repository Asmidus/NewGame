#pragma once
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <string>



enum WindowFlags {
	INVISIBLE = 0x1,
	FULLSCREEN = 0x2,
	BORDERLESS = 0x4,
	VSYNC = 0x8
};

class Window {
public:
	Window();
	~Window();

	void create(std::string windowName, int screenWidth, int screenHeight, unsigned int currentFlags);
	void swapBuffer();
	SDL_Window* get() { return _window; }
	int getScreenWidth() { return _screenWidth; }
	int getScreenHeight() { return _screenHeight; }
private:
	SDL_Window* _window;
	int _screenWidth, _screenHeight;
};