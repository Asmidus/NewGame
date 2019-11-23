#include "EventManager.h"
#include "Components.h"
#include "AssetManager.h"
#include "entt/entt.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>



EventManager::~EventManager() {}

void EventManager::registerEvent(Event event) {
	_events.push_back(event);
}

int EventManager::processEvents(float dt) {
	_dt = dt;
	for (auto event : _events) {
		switch (event.type) {
		case Event::shootBullet:
			processShoot(event);
			break;
		case Event::Type::collision:
			processCollision(event);
			break;
		case Event::Type::moveUp:
		case Event::Type::moveRight:
		case Event::Type::moveLeft:
			processMove(event);
			break;
		case Event::Type::startGame:
			processStartGame(event);
			break;
		case Event::Type::quit:
			if (processQuit(event)) {
				return 1;
			}
			break;
		}
	}
	_events.clear();
	return 0;
}

void EventManager::processCollision(Event& event) {
	entt::entity p, e;
	//for (int i = 0; i < event.entities.size(); i++) {
	//	entt::entity collided = event.entities.at(i);
	//	if (!_registry->valid(collided)) {
	//		return;
	//	}
	//	auto& collider = _registry->get<Collider>(collided);
	//	auto health = _registry->try_get<Health>(collided);
	//	if (!_registry->has<entt::tag<"Player"_hs>>(collided)) {
	//		//if (_registry->has<entt::tag<"Split"_hs>>(collided)) {
	//		//	for (int i = 0; i < rand() % 2 + 2; i++) {
	//		//		AssetManager::createAsteroid(&collided);
	//		//	}
	//		//}
	//		e = collided;
	//		auto& velocity = _registry->get<Velocity>(collided);
	//		//velocity.currVel *= -1;
	//		//velocity.direction *= -1;
	//		//_registry->destroy(collided);
	//	} else {
	//		p = collided;
	//		if (health) {
	//			//health->current -= 1.0f;
	//			if (health->current > 0) {
	//				auto& sprite = _registry->get<Sprite>(collided);
	//				sprite.color = { 255, GLubyte(255 * health->current / health->max), GLubyte(255 * health->current / health->max), 255 };
	//				continue;
	//			}
	//			AssetManager::clearScreen();
	//			AssetManager::createMenu();
	//			return;
	//		}
	//		//_registry->destroy(collided);
	//	}
	//}
	p = event.entities[0];
	e = event.entities[1];
	if (!_registry->valid(p) || !_registry->valid(e)) {
		return;
	}
	if (_registry->has<entt::tag<"Player"_hs>>(e)) {
		auto n = p;
		p = e;
		e = n;
	}
	auto& v1 = _registry->get<Velocity>(p);
	auto& v2 = _registry->get<Velocity>(e);
	auto& t1 = _registry->get<Transform>(p);
	auto& t2 = _registry->get<Transform>(e);
	auto& c1 = _registry->get<Collider>(p);
	auto& c2 = _registry->get<Collider>(e);
	glm::vec2 e1Pos = glm::vec2(t1.center.x * t1.rect.w + t1.rect.x,
								t1.center.y * t1.rect.h + t1.rect.y);
	glm::vec2 e2Pos = glm::vec2(t2.center.x * t2.rect.w + t2.rect.x,
								t2.center.y * t2.rect.h + t2.rect.y);
	auto dir = e1Pos - e2Pos;
	if (e1Pos == e2Pos) {
		dir = glm::vec2(rand()%200-100, rand()%200-100);
	}
	auto speed = glm::length(v2.currVel);
	float depth = c1.radius + c2.radius - glm::length(dir) + 0.5;
	if (e1Pos == e2Pos) {
		depth += 1;
	}
	glm::vec2 displace = glm::normalize(dir) * depth;
	v2.currVel = -glm::normalize(dir) * speed;
	if (!_registry->has<Health>(p)) {
		v1.currVel = glm::normalize(dir) * glm::length(v1.currVel);
		t2.rect.x -= displace.x/2;
		t2.rect.y -= displace.y/2;
		t1.rect.x += displace.x/2;
		t1.rect.y += displace.y/2;
	} else {
		t2.rect.x -= displace.x;
		t2.rect.y -= displace.y;
	}
	if (_registry->has<entt::tag<"Player"_hs>>(p) && !_registry->has<entt::tag<"Player"_hs>>(e)) {
		if (_registry->has<entt::tag<"Split"_hs>>(e)) {
			for (int i = 0; i < rand() % 2 + 2; i++) {
				AssetManager::createAsteroid(&e);
			}
		}
		_registry->destroy(e);
	}
}

void EventManager::processMove(Event& event) {
	for (auto entity : event.entities) {
		auto& entityVel = _registry->get<Velocity>(entity);
		auto animation = _registry->try_get<Animation>(entity);
		auto& direction = entityVel.direction;
		auto& transform = _registry->get<Transform>(entity);
		switch (event.type) {
		case Event::moveUp:
			if (animation) {
				animation->active = true;
			}
			entityVel.currAccel = entityVel.accel;
			break;
		case Event::moveRight:
			direction = glm::rotate<float>(direction, 6*_dt);
			break;
		case Event::moveLeft:
			direction = glm::rotate<float>(direction, -6*_dt);
			break;
		}
	}
}

void EventManager::processShoot(Event& event) {
	auto bullet = AssetManager::createBullet(event.entities[0]);
}

void EventManager::processStartGame(Event& event) {
	if (!event.entities.empty()) {
		auto rect = _registry->get<Transform>(event.entities[0]).rect;
		auto pos = event.mousePos;
		if (pos.x < rect.x || pos.x > rect.x + rect.w ||
			pos.y < rect.y || pos.y > rect.y + rect.h) {
			return;
		}
	}
	AssetManager::clearScreen();
	AssetManager::createPlayer();
	AssetManager::createAsteroidSpawner();
}

int EventManager::processQuit(Event& event) {
	if (!event.entities.empty()) {
		auto rect = _registry->get<Transform>(event.entities[0]).rect;
		auto pos = event.mousePos;
		if (pos.x < rect.x || pos.x > rect.x + rect.w ||
			pos.y < rect.y || pos.y > rect.y + rect.h) {
			return 0;
		}
	}
	return 1;
}