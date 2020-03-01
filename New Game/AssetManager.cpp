#include <SDL.h>
#include <iostream>
#include "AssetManager.h"
#include "Components.h"
#include "Global.h"



entt::entity AssetManager::createPlayer() {
	auto entity = Global::registry.create();
	static unsigned int shipSize = 30;
	std::unordered_map<unsigned int, std::function<bool(bool)>> keyMap;
	std::unordered_map<Event::Type, float> cooldowns;
	cooldowns[Event::Type::shootBullet] = 0.25f;
	cooldowns[Event::Type::damage] = 1.5f;
	//cooldowns[Event::Type::collision] = 1.5f;
	Velocity* vel = &(Global::registry.assign<Velocity>(entity, glm::vec2(0, 0), 30, 2));
	Global::registry.assign<Sprite>(entity, "media/ECSplayer.png", 50, 50);
	Global::registry.assign<Transform>(entity,
								 Global::gameWidth/2 - shipSize/2, Global::gameHeight/2 - shipSize/2,	//pos
								 shipSize, shipSize,										//size
								 0,															//z level
								 0.4f, 0.5f);												//center
	Cooldown* cool = &(Global::registry.assign<Cooldown>(entity, cooldowns));
	Global::registry.assign<Animation>(entity, 5, 0.08, false);
	Global::registry.assign<Health>(entity, 5.0f);
	Global::registry.assign<Collider>(entity, shipSize/2, INT_MAX);
	Global::registry.assign<entt::tag<"Player"_hs>>(entity);
	Global::registry.assign<entt::tag<"SpriteHealth"_hs>>(entity);
	//Global::registry.assign<entt::tag<"Occluder"_hs>>(entity);
	//Global::registry.assign<entt::tag<"Bright"_hs>>(entity);
	keyMap[SDLK_w] = [entity](bool pressed) {
		auto& velocity = Global::registry.get<Velocity>(entity);
		auto& animation = Global::registry.get<Animation>(entity);
		if (pressed) {
			velocity.direction.y -= 1;
		} else {
			velocity.direction.y += 1;
		}
		return false;
	};
	keyMap[SDLK_s] = [entity](bool pressed) {
		auto& velocity = Global::registry.get<Velocity>(entity);
		auto& animation = Global::registry.get<Animation>(entity);
		if (pressed) {
			velocity.direction.y += 1;
		} else {
			velocity.direction.y -= 1;
		}
		return false;
	};
	keyMap[SDLK_d] = [entity](bool pressed) {
		auto& velocity = Global::registry.get<Velocity>(entity);
		if (pressed) {
			velocity.direction.x += 1;
		} else {
			velocity.direction.x -= 1;
		}
		return false;
	};
	keyMap[SDLK_a] = [entity](bool pressed) {
		auto& velocity = Global::registry.get<Velocity>(entity);
		if (pressed) {
			velocity.direction.x -= 1;
		} else {
			velocity.direction.x += 1;
		}
		return false;
	};
	keyMap[SDLK_SPACE] = [entity, cool](bool pressed) {
		if (pressed && cool->trigger(Event::shootBullet)) {
			//for(int i = 0;i < 500;i++)
				createBullet(entity);
			//createBasicEnemy();
		}
		return true;
	};
	Global::registry.assign<KeyListener>(entity, keyMap);
	//Global::collisionTree.insertObject(entity);

	//hacky background for lights to render on
	//auto entity2 = Global::registry.create();
	//Global::registry.assign<Sprite>(entity2, "media/Button.png", 160, 100, glm::vec3(10, 10, 10));
	//Global::registry.assign<Transform>(entity2, 0, 0, Global::gameWidth, Global::gameHeight, 3);
	return entity;
}

entt::entity AssetManager::createBasicEnemy() {
	auto entity = Global::registry.create();
	static unsigned int shipSize = 30;
	std::unordered_map<Event::Type, float> cooldowns;
	//cooldowns[Event::Type::shootBullet] = 0.005f;
	//cooldowns[Event::Type::collision] = 1.5f;
	Velocity* vel = &(Global::registry.assign<Velocity>(entity, glm::vec2(-1, 0), 10, 5, 0));
	float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));

	Global::registry.assign<Sprite>(entity, "media/Projectile.png", 50, 50);
	Global::registry.assign<Transform>(entity,
								 Global::gameWidth, Global::gameHeight * r,	//pos
								 shipSize, shipSize,										//size
								 0,															//z level
								 0.4f, 0.5f);												//center
	//Cooldown* cool = &(Global::registry.assign<Cooldown>(entity, cooldowns));
	Global::registry.assign<Animation>(entity, 5, 0.08, false);
	Global::registry.assign<Health>(entity, 5.0f);
	Global::registry.assign<Collider>(entity, shipSize / 2, 1);
	Global::registry.assign<entt::tag<"Enemy"_hs>>(entity);
	return entity;
}

entt::entity AssetManager::createBullet(const entt::entity& shooter) {
	auto entity = Global::registry.create();
	auto shooterTransform = Global::registry.get<Transform>(shooter);
	static float bulletSize = 50;
	glm::vec2 point = glm::vec2(shooterTransform.rect.w, shooterTransform.rect.h/2);
	float angle = shooterTransform.angle;
	auto center = shooterTransform.center * glm::vec2(shooterTransform.rect.w, shooterTransform.rect.h);
	float rotatedX = cos(angle) * (point.x - center.x) - sin(angle) * (point.y - center.y) + center.x + shooterTransform.rect.x - bulletSize/2;
	float rotatedY = sin(angle) * (point.x - center.x) + cos(angle) * (point.y - center.y) + center.y + shooterTransform.rect.y - bulletSize/2;
	if (Global::registry.size() > 5) {
		Global::registry.assign<Velocity>(entity, glm::vec2(1, 0), 0.0f);
	} else {
		Global::registry.assign<Velocity>(entity, glm::vec2(-1, 0), 2.0f);
	}
	Global::registry.get<Velocity>(entity).maxSpeed = 3;
	Global::registry.assign<Transform>(entity, rotatedX, rotatedY, bulletSize, bulletSize, 1);
	float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
	float g = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
	float b = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
	Global::registry.assign<Sprite>(entity, "media/Projectile.png", 50, 50, glm::vec3(255*r, 255*g, 255*b));
	Global::registry.assign<Lifetime>(entity, 5);
	Global::registry.assign<Collider>(entity, bulletSize/2, 1);
	//Global::registry.assign<Health>(entity, 1);
	//Global::collisionTree.insertObject(entity);
	//Global::registry.assign<entt::tag<"Bright"_hs>>(entity);
	//float size = 1000 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2000 - 1000.0f)));
	//Global::registry.assign<Light>(entity, glm::vec3(r, g, b), 4.0f);
	Global::registry.assign<entt::tag<"Player"_hs>>(entity);
	return entity;
}

entt::entity AssetManager::createAsteroid(glm::vec2 speedRange, glm::vec2 sizeRange) {
	auto entity = Global::registry.create();
	int xDir = 0, yDir = 0;
	while (!xDir || !yDir) {
		xDir = rand() % 200 - 100;
		yDir = rand() % 200 - 100;
	}
	//bool topSpawn = rand() % 2;
	float x, y;
	//if (topSpawn) {
	x = rand() % int(Global::gameWidth);
	//y = -99 + rand() % 2 * (Global::gameHeight + 99);
	//} else {
		//x = -99 + rand() % 2 * (Global::gameWidth + 99);
	y = rand() % int(Global::gameHeight);
	//}
	float speed = speedRange.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (speedRange.y - speedRange.x)));
	float size = sizeRange.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (sizeRange.y - sizeRange.x)));
	Global::registry.assign<Velocity>(entity, glm::vec2(xDir, yDir), speed);
	Global::registry.assign<Transform>(entity, x, y, size, size, 2);
	Global::registry.assign<entt::tag<"Split"_hs>>(entity);
	Global::registry.assign<Collider>(entity, size / 2);
	Global::registry.assign<Sprite>(entity, "media/Projectile.png", 50, 50, glm::vec3(255, 255, 255));
	Global::registry.assign<entt::tag<"Enemy"_hs>>(entity);
	//Global::registry.assign<entt::tag<"Occluder"_hs>>(entity);
	return entity;
}

entt::entity AssetManager::createAsteroid(entt::entity* parentAsteroid) {
	auto entity = Global::registry.create();
	int xDir = 0, yDir = 0;
	while (!xDir || !yDir) {
		xDir = rand() % 200 - 100;
		yDir = rand() % 200 - 100;
	}
	auto transform = Global::registry.get<Transform>(*parentAsteroid);
	float speed = Global::registry.get<Velocity>(*parentAsteroid).maxSpeed + 0.5f;
	Global::registry.assign<Velocity>(entity, glm::vec2(xDir, yDir), speed);
	Global::registry.assign<Transform>(entity, transform.rect.x, transform.rect.y, transform.rect.w / 2, transform.rect.h / 2, 2);
	if (transform.rect.w > 75) {
		Global::registry.assign<entt::tag<"Split"_hs>>(entity);
	}
	Global::registry.assign<Collider>(entity, Global::registry.get<Collider>(*parentAsteroid).radius/2);
	Global::registry.assign<Sprite>(entity, "media/Projectile.png", 50, 50, glm::vec3(255, 75, 0));
	Global::registry.assign<entt::tag<"Enemy"_hs>>(entity);
	//Global::registry.assign<entt::tag<"Occluder"_hs>>(entity);
	return entity;
}

entt::entity AssetManager::createButton(Event::Type type, const char* text) {
	auto entity = Global::registry.create();
	std::unordered_map<unsigned int, Event::Type> mouseMap;
	mouseMap[SDL_BUTTON_LEFT] = type;
	Global::registry.assign<Sprite>(entity, "media/Button.png", 160, 100, glm::vec3(255, 100, 100));
	Global::registry.assign<Transform>(entity, 0, 0, 160, 100, 0);
	Global::registry.assign<MouseListener>(entity, mouseMap);
	//Global::registry.assign<entt::tag<"Bright"_hs>>(entity);
	Global::registry.assign<Text>(entity, text, 160, 100, 32, Color(glm::vec4({ 100, 100, 100, 155 })));
	return entity;
}

entt::entity AssetManager::createAsteroidSpawner() {
	auto entity = Global::registry.create();
	Global::registry.assign<AsteroidSpawner>(entity, 6, 2, glm::vec2(0.25, 1), glm::vec2(75, 150));
	return entity;
}

void AssetManager::clearScreen() {
	Global::registry.each([](auto entity) {
		Global::registry.destroy(entity);
	});
}

void AssetManager::createMenu() {
	auto start = createButton(Event::Type::startGame, "START");
	auto& startRect = Global::registry.get<Transform>(start).rect;
	startRect.x = Global::gameWidth / 4 - startRect.w / 2;
	startRect.y = Global::gameHeight / 2 - startRect.h / 2;
	auto quit = createButton(Event::Type::quit, "QUIT");
	auto& quitRect = Global::registry.get<Transform>(quit).rect;
	quitRect.x = Global::gameWidth * 3 / 4 - quitRect.w / 2;
	quitRect.y = Global::gameHeight / 2 - quitRect.h / 2;
}