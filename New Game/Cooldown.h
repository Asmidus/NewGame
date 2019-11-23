#pragma once
#include <unordered_map>
#include "Event.h"

//A cooldown is a restriction on an entity that prevents it from triggering an event for the specified amount of time
//For example - the player entity not being able to shoot or collide with other entities 500 times a second
struct Cooldown {
	std::unordered_map<Event::Type, float> events;
	std::unordered_map<Event::Type, float> coolingDown;

	Cooldown(std::unordered_map<Event::Type, float> eventMap) {
		events = eventMap;
	}

	void decrementCooldowns(float dt) {
		for (auto it = coolingDown.begin(); it != coolingDown.end();) {
			if (it->second - dt <= 0) {
				coolingDown.erase(it++);
			} else {
				it++->second -= dt;
			}
		}
	}

	bool trigger(Event::Type type) {
		if (coolingDown.find(type) == coolingDown.end()) {
			if (events.find(type) != events.end()) {
				coolingDown[type] = events[type];
			}
			return true;
		}
		return false;
	}
};