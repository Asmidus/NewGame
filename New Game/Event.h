#pragma once
#include "entt/entt.hpp"
#include <glm/glm.hpp>

//Events are essentially actions that are triggered from something.
//They can occur when two objects collide or when the user presses a button
struct Event {
public:
	enum Type {
		collision,
		damage,
		button,
		moveUp,
		moveRight,
		moveLeft,
		shootBullet,
		startGame,
		quit
	};

	Event(const Type& type, const std::vector<entt::entity>& entities, const glm::vec2& mp = { 0, 0 }) : type(type), entities(entities), mousePos(mp) {}
	Event(const Type& type, const entt::entity& entity, const glm::vec2& mp = { 0, 0 }) : type(type), entities({ entity }), mousePos(mp) {}

	//the list of entities effected by the event
	std::vector<entt::entity> entities;
	//the type of event
	Type type;
	//the position the event was sent from if applicable
	glm::vec2 mousePos;
	//other data
	void* data;
};