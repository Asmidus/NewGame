#include "InputManager.h"
#include <algorithm>



InputManager::InputManager() : _mouseCoords(0.0f) {}

InputManager::~InputManager() {}

void InputManager::pressKey(unsigned int keyID) {
	if (std::find(_pressedKeys.begin(), _pressedKeys.end(), keyID) == _pressedKeys.end()) {
		_pressedKeys.push_back(keyID);
	}
}

void InputManager::releaseKey(unsigned int keyID) {
	_pressedKeys.remove(keyID);
	_releasedKeys.push_back(keyID);
}

void InputManager::setMouseCoords(float x, float y) {
	_mouseCoords.x = x;
	_mouseCoords.y = y;
}