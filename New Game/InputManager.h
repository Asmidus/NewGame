#pragma once
#include <list>
#include <glm/glm.hpp>

class InputManager {
public:
	InputManager();
	~InputManager();
		
	void pressKey(unsigned int keyID);
	void releaseKey(unsigned int keyID);
	void setMouseCoords(float x, float y);
	void refresh() { _releasedKeys.clear(); }
	glm::vec2 getMouseCoords() const { return _mouseCoords; }
	const std::list<unsigned int>& getPressedKeys() { return _pressedKeys; }
	const std::list<unsigned int>& getReleasedKeys() { return _releasedKeys; }

private:
	std::list<unsigned int> _pressedKeys;
	std::list<unsigned int> _releasedKeys;
	glm::vec2 _mouseCoords;
};