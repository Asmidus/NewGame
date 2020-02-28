#include "EventManager.h"
#include "Components.h"
#include "AssetManager.h"
#include "entt/entt.hpp"
#include "Global.h"
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
	p = event.entities[0];
	e = event.entities[1];
	if (!Global::registry.valid(p) || !Global::registry.valid(e)) {
		return;
	}
	if (Global::registry.has<entt::tag<"Player"_hs>>(e)) {
		auto n = p;
		p = e;
		e = n;
	}
	auto [pv, pt, pc] = Global::registry.get<Velocity, Transform, Collider>(p);
	auto [ev, et, ec] = Global::registry.get<Velocity, Transform, Collider>(e);
	auto ph = Global::registry.try_get<Health>(p);
	auto pCool = Global::registry.try_get<Cooldown>(p);
	auto eh = Global::registry.try_get<Health>(e);
	auto eCool = Global::registry.try_get<Cooldown>(e);
	glm::vec2 e1Pos = glm::vec2(pt.center.x * pt.rect.w + pt.rect.x,
								pt.center.y * pt.rect.h + pt.rect.y);
	glm::vec2 e2Pos = glm::vec2(et.center.x * et.rect.w + et.rect.x,
								et.center.y * et.rect.h + et.rect.y);
	auto dir = e1Pos - e2Pos;
	if (e1Pos == e2Pos) {
		dir = glm::vec2(rand()%200-100, rand()%200-100);
	}
	auto speed = glm::length(ev.currVel);
	float depth = pc.radius + ec.radius - glm::length(dir);
	if (e1Pos == e2Pos) {
		depth += 1;
	}
	glm::vec2 displace = glm::normalize(dir) * depth;
	//ev.currVel = -glm::normalize(dir) * speed;
	float pFactor;
	float eFactor;
	float totalMass = pc.mass + ec.mass;
	if (pc.mass == 0 && ec.mass == 0) {
		pFactor = eFactor = 2;
	} else if (pc.mass == 0) {
		pFactor = 0.00001;
		eFactor = 0.99999;
	} else if (ec.mass == 0) {
		pFactor = 0.99999;
		eFactor = 0.00001;
	} else if (pc.mass == INT_MAX) {
		pFactor = 1;
		eFactor = 0;
	} else if (ec.mass == INT_MAX) {
		pFactor = 0;
		eFactor = 1;
	} else {
		pFactor = pc.mass / totalMass;
		eFactor = ec.mass / totalMass;
	}
	float force = glm::length(pv.currVel) + glm::length(ev.currVel);
	//pv.currVel = glm::normalize(pv.currVel - 2 * glm::dot(pv.currVel, glm::normalize(dir)) * glm::normalize(dir)) * force * (1 - pFactor);
	//ev.currVel = glm::normalize(ev.currVel - 2 * glm::dot(ev.currVel, glm::normalize(dir)) * glm::normalize(dir)) * force * (1 - eFactor);
	pv.currVel += glm::normalize(dir) * force * (1 - pFactor);
	ev.currVel -= glm::normalize(dir) * force * (1 - eFactor);
	et.rect.x -= displace.x * (1 - eFactor);
	et.rect.y -= displace.y * (1 - eFactor);
	pt.rect.x += displace.x * (1 - pFactor);
	pt.rect.y += displace.y * (1 - pFactor);

	if (Global::registry.has<entt::tag<"Player"_hs>>(p) && Global::registry.has<entt::tag<"Enemy"_hs>>(e)) {
		if (eh && (!eCool || eCool->trigger(Event::Type::damage))) {
			eh->current -= 1;
		}
		if (ph && (!pCool || pCool->trigger(Event::Type::damage))) {
			ph->current -= 1;
		}
	}
}

void EventManager::processMove(Event& event) {
	for (auto entity : event.entities) {
		auto& entityVel = Global::registry.get<Velocity>(entity);
		auto animation = Global::registry.try_get<Animation>(entity);
		auto& direction = entityVel.direction;
		auto& transform = Global::registry.get<Transform>(entity);
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
		auto rect = Global::registry.get<Transform>(event.entities[0]).rect;
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
		auto rect = Global::registry.get<Transform>(event.entities[0]).rect;
		auto pos = event.mousePos;
		if (pos.x < rect.x || pos.x > rect.x + rect.w ||
			pos.y < rect.y || pos.y > rect.y + rect.h) {
			return 0;
		}
	}
	return 1;
}