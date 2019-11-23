#pragma once
#include "entt/entt.hpp"
#include "Event.h"

namespace AssetManager {
	void init(entt::registry* r, glm::vec2& gameDims);
	entt::entity createPlayer();
	entt::entity createBullet(const entt::entity& shooter);
	entt::entity createAsteroid(glm::vec2 speedRange, glm::vec2 sizeRange);
	entt::entity createAsteroid(entt::entity* parentAsteroid);
	entt::entity createButton(Event::Type type, const char* text);
	entt::entity createAsteroidSpawner();
	void clearScreen();
	void createMenu();
}