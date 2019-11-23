#pragma once
#include <SDL.h>
#include <unordered_map>
#include "Event.h"

struct KeyListener {
	KeyListener(std::unordered_map<unsigned int, std::function<bool(bool)>> inputMap) : map(inputMap) {
		for (auto key : map) {
			enabled[key.first] = true;
		}
	}
	/*The function that the key corresponds to should have a signature like this:
		bool fun(bool pressed);
		where pressed is whether or not the key corresponding to this function was pressed (versus being released)
		the bool the function returns is whether or not this function repeats while the key is held
	*/
	std::unordered_map<unsigned int, std::function<bool(bool)>> map;
	std::unordered_map<unsigned int, bool> enabled;
	bool active = true;
};