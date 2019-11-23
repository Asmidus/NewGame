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
	Systems(entt::registry* registry, EventManager* events, InputManager* inputs) : _registry(registry), _events(events), _inputs(inputs), _camera(nullptr), _program(nullptr), _gameWidth(0), _gameHeight(0) {}
	~Systems() {}
	void init(Program* program, Camera* camera, float screenWidth, float screenHeight) { _gameWidth = screenWidth; _gameHeight = screenHeight; _program = program; _camera = camera; _lightEngine.LoadShaders(); }
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
	float _gameWidth, _gameHeight;
	entt::registry* _registry;
	EventManager* _events;
	InputManager* _inputs;
	Program* _program;
	Camera* _camera;
	LightEngine _lightEngine;
};

