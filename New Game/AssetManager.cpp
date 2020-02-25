#include <SDL.h>
#include <iostream>
#include "AssetManager.h"
#include "Components.h"
#include "Global.h"



static entt::registry* _registry = nullptr;

void AssetManager::init(entt::registry* r) {
	_registry = r;
}

entt::entity AssetManager::createPlayer() {
	auto entity = _registry->create();
	static unsigned int shipSize = 30;
	std::unordered_map<unsigned int, std::function<bool(bool)>> keyMap;
	std::unordered_map<Event::Type, float> cooldowns;
	cooldowns[Event::Type::shootBullet] = 0.005f;
	//cooldowns[Event::Type::collision] = 1.5f;
	Velocity* vel = &(_registry->assign<Velocity>(entity, glm::vec2(0, 0), 30, 2));
	_registry->assign<Sprite>(entity, "media/ECSplayer.png", 50, 50);
	_registry->assign<Transform>(entity,
								 Global::gameWidth/2 - shipSize/2, Global::gameHeight/2 - shipSize/2,	//pos
								 shipSize, shipSize,										//size
								 0,															//z level
								 0.4f, 0.5f);												//center
	Cooldown* cool = &(_registry->assign<Cooldown>(entity, cooldowns));
	_registry->assign<Animation>(entity, 5, 0.08, false);
	_registry->assign<Health>(entity, 5.0f);
	_registry->assign<Collider>(entity, shipSize/2);
	_registry->assign<entt::tag<"Player"_hs>>(entity);
	//_registry->assign<entt::tag<"Occluder"_hs>>(entity);
	//_registry->assign<entt::tag<"Bright"_hs>>(entity);
	keyMap[SDLK_w] = [entity](bool pressed) {
		auto& velocity = _registry->get<Velocity>(entity);
		auto& animation = _registry->get<Animation>(entity);
		if (pressed) {
			velocity.direction.y -= 1;
		} else {
			velocity.direction.y += 1;
		}
		return false;
	};
	keyMap[SDLK_s] = [entity](bool pressed) {
		auto& velocity = _registry->get<Velocity>(entity);
		auto& animation = _registry->get<Animation>(entity);
		if (pressed) {
			velocity.direction.y += 1;
		} else {
			velocity.direction.y -= 1;
		}
		return false;
	};
	keyMap[SDLK_d] = [entity](bool pressed) {
		auto& velocity = _registry->get<Velocity>(entity);
		if (pressed) {
			velocity.direction.x += 1;
		} else {
			velocity.direction.x -= 1;
		}
		return false;
	};
	keyMap[SDLK_a] = [entity](bool pressed) {
		auto& velocity = _registry->get<Velocity>(entity);
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
				//createBullet(entity);
			createBasicEnemy();
		}
		return true;
	};
	_registry->assign<KeyListener>(entity, keyMap);

	//hacky background for lights to render on
	//auto entity2 = _registry->create();
	//_registry->assign<Sprite>(entity2, "media/Button.png", 160, 100, glm::vec3(10, 10, 10));
	//_registry->assign<Transform>(entity2, 0, 0, Global::gameWidth, Global::gameHeight, 3);
	return entity;
}

entt::entity AssetManager::createBasicEnemy() {
	auto entity = _registry->create();
	static unsigned int shipSize = 30;
	std::unordered_map<Event::Type, float> cooldowns;
	//cooldowns[Event::Type::shootBullet] = 0.005f;
	//cooldowns[Event::Type::collision] = 1.5f;
	Velocity* vel = &(_registry->assign<Velocity>(entity, glm::vec2(-1, 0), 10, 5, 0));
	float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));

	_registry->assign<Sprite>(entity, "media/Projectile.png", 50, 50);
	_registry->assign<Transform>(entity,
								 Global::gameWidth, Global::gameHeight * r,	//pos
								 shipSize, shipSize,										//size
								 0,															//z level
								 0.4f, 0.5f);												//center
	//Cooldown* cool = &(_registry->assign<Cooldown>(entity, cooldowns));
	_registry->assign<Animation>(entity, 5, 0.08, false);
	_registry->assign<Health>(entity, 5.0f);
	_registry->assign<Collider>(entity, shipSize / 2);
	_registry->assign<entt::tag<"Enemy"_hs>>(entity);
	return entity;
}

entt::entity AssetManager::createBullet(const entt::entity& shooter) {
	auto entity = _registry->create();
	auto shooterTransform = _registry->get<Transform>(shooter);
	static float bulletSize = 12.5;
	glm::vec2 point = glm::vec2(shooterTransform.rect.w, shooterTransform.rect.h/2);
	float angle = shooterTransform.angle;
	auto center = shooterTransform.center * glm::vec2(shooterTransform.rect.w, shooterTransform.rect.h);
	float rotatedX = cos(angle) * (point.x - center.x) - sin(angle) * (point.y - center.y) + center.x + shooterTransform.rect.x - bulletSize/2;
	float rotatedY = sin(angle) * (point.x - center.x) + cos(angle) * (point.y - center.y) + center.y + shooterTransform.rect.y - bulletSize/2;
	_registry->assign<Velocity>(entity, glm::vec2(1, 0), 2.0f);
	_registry->assign<Transform>(entity, rotatedX, rotatedY, bulletSize, bulletSize, 1);
	float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
	float g = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
	float b = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
	_registry->assign<Sprite>(entity, "media/Projectile.png", 50, 50, glm::vec3(255*r, 255*g, 255*b));
	//_registry->assign<Lifetime>(entity, 3);
	_registry->assign<Collider>(entity, bulletSize/2);
	//_registry->assign<entt::tag<"Bright"_hs>>(entity);
	//float size = 1000 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2000 - 1000.0f)));
	//_registry->assign<Light>(entity, glm::vec3(r, g, b), 4.0f);
	_registry->assign<entt::tag<"Player"_hs>>(entity);
	return entity;
}

entt::entity AssetManager::createAsteroid(glm::vec2 speedRange, glm::vec2 sizeRange) {
	auto entity = _registry->create();
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
	_registry->assign<Velocity>(entity, glm::vec2(xDir, yDir), speed);
	_registry->assign<Transform>(entity, x, y, size, size, 2);
	_registry->assign<entt::tag<"Split"_hs>>(entity);
	_registry->assign<Collider>(entity, size / 2);
	_registry->assign<Sprite>(entity, "media/Projectile.png", 50, 50, glm::vec3(255, 255, 255));
	_registry->assign<entt::tag<"Enemy"_hs>>(entity);
	//_registry->assign<entt::tag<"Occluder"_hs>>(entity);
	return entity;
}

entt::entity AssetManager::createAsteroid(entt::entity* parentAsteroid) {
	auto entity = _registry->create();
	int xDir = 0, yDir = 0;
	while (!xDir || !yDir) {
		xDir = rand() % 200 - 100;
		yDir = rand() % 200 - 100;
	}
	auto transform = _registry->get<Transform>(*parentAsteroid);
	float speed = _registry->get<Velocity>(*parentAsteroid).maxSpeed + 0.5f;
	_registry->assign<Velocity>(entity, glm::vec2(xDir, yDir), speed);
	_registry->assign<Transform>(entity, transform.rect.x, transform.rect.y, transform.rect.w / 2, transform.rect.h / 2, 2);
	if (transform.rect.w > 75) {
		_registry->assign<entt::tag<"Split"_hs>>(entity);
	}
	_registry->assign<Collider>(entity, _registry->get<Collider>(*parentAsteroid).radius/2);
	_registry->assign<Sprite>(entity, "media/Projectile.png", 50, 50, glm::vec3(255, 75, 0));
	_registry->assign<entt::tag<"Enemy"_hs>>(entity);
	//_registry->assign<entt::tag<"Occluder"_hs>>(entity);
	return entity;
}

entt::entity AssetManager::createButton(Event::Type type, const char* text) {
	auto entity = _registry->create();
	std::unordered_map<unsigned int, Event::Type> mouseMap;
	mouseMap[SDL_BUTTON_LEFT] = type;
	_registry->assign<Sprite>(entity, "media/Button.png", 160, 100, glm::vec3(255, 100, 100));
	_registry->assign<Transform>(entity, 0, 0, 160, 100, 0);
	_registry->assign<MouseListener>(entity, mouseMap);
	//_registry->assign<entt::tag<"Bright"_hs>>(entity);
	_registry->assign<Text>(entity, text, 160, 100, 32, Color(glm::vec4({ 100, 100, 100, 155 })));
	return entity;
}

entt::entity AssetManager::createAsteroidSpawner() {
	auto entity = _registry->create();
	_registry->assign<AsteroidSpawner>(entity, 6, 2, glm::vec2(0.25, 1), glm::vec2(75, 150));
	return entity;
}

void AssetManager::clearScreen() {
	_registry->each([](auto entity) {
		_registry->destroy(entity);
	});
}

void AssetManager::createMenu() {
	auto start = createButton(Event::Type::startGame, "START");
	auto& startRect = _registry->get<Transform>(start).rect;
	startRect.x = Global::gameWidth / 4 - startRect.w / 2;
	startRect.y = Global::gameHeight / 2 - startRect.h / 2;
	auto quit = createButton(Event::Type::quit, "QUIT");
	auto& quitRect = _registry->get<Transform>(quit).rect;
	quitRect.x = Global::gameWidth * 3 / 4 - quitRect.w / 2;
	quitRect.y = Global::gameHeight / 2 - quitRect.h / 2;
}