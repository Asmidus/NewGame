#pragma once
#include "entt/entt.hpp"
#include <GL/glew.h>
#include "InputManager.h"
#include "LightEngine.h"

class EventManager;
class SpriteBatch;
class Camera;
class Program;
class Systems
{
public:
	Systems(entt::registry* registry, EventManager* events, InputManager* inputs) :
		_registry(registry), _events(events), _inputs(inputs), _camera(nullptr), _program(nullptr) {}
	~Systems() {}
	void init(Program* program, Camera* camera) {
		_program = program;
		_camera = camera;
		_lightEngine.LoadShaders();
	}
	void updateDelta(float dt) { _dt = dt; }
	void drawSprites(SpriteBatch* batch);
	void updateAnimations();
	void moveEntities();
	void checkCollisions();
	void checkInput(Camera* currCam);
	void checkLifetimes();
	void spawnAsteroids();
private:
	float _dt;
	entt::registry* _registry;
	EventManager* _events;
	InputManager* _inputs;
	Program* _program;
	Camera* _camera;
	LightEngine _lightEngine;
};

