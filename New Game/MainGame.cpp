#include "MainGame.h"
#include <iostream>
#include "InputManager.h"
#include <string>
#include <random>
#include <time.h>
#include <string>
#include "Transform.h"
#include "Velocity.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Global.h"



float Global::screenWidth = 750;
float Global::screenHeight = 750;
float Global::gameWidth = 1000;
float Global::gameHeight = 1000;
entt::registry Global::registry;
//AABBTree Global::collisionTree(10000);
static GLuint texLoc, camLoc;

MainGame::MainGame() :
_gameState(GameState::PLAY), _fpsLimiter(60.0f), _fps(120.0f), _frameTime(0),
_events(), _systems(&_events, &_inputManager) {}

MainGame::~MainGame() {
}

void MainGame::run() {
	initSystems();
	AssetManager::createPlayer();
	gameLoop();
}

void MainGame::initSystems() {
	//Initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	_window.create("ECStroids", Global::screenWidth, Global::screenHeight, 0);
	_systems.init(&_program, &_camera);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	_camera.init(Global::screenWidth, Global::screenHeight);
	_camera.setPosition(glm::vec2(Global::gameWidth, Global::gameHeight) / 2.0f);
	_camera.setScale(Global::screenHeight/Global::gameHeight);
	//_camera.setScale(10);

	//Shaders
	_program.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	_program.addAttribute("vertexPosition");
	_program.addAttribute("vertexColor");
	_program.addAttribute("vertexUV");
	_program.link();
	_program.use();
	texLoc = _program.getUniformLocation("mySampler");
	camLoc = _program.getUniformLocation("P");

	_batch.init();
	srand(time(0));
}

void MainGame::initLevel() {

}

void MainGame::gameLoop() {
	while (_gameState != GameState::EXIT) {
		_fpsLimiter.begin();
		_systems.updateDelta(1 / _fps);
		_systems.checkLifetimes();
		_systems.spawnAsteroids();
		_camera.update();
		_systems.checkInput(&_camera);
		_systems.moveEntities();
		drawGame();
		if (_events.processEvents(1 / _fps)) {
			break;
		}
		_systems.checkCollisions();
		static unsigned int loop = 0;
		if (loop % 10 == 0) {
			loop = 1;
			int asteroids = Global::registry.view<entt::tag<"Enemy"_hs>>().size();
			int bullets = Global::registry.view<Sprite>().size();
			SDL_SetWindowTitle(_window.get(), std::string("ECStroids - FPS: " + std::to_string(_fps) + " Asteroids: " + std::to_string(asteroids) + " Bullets: " + std::to_string(bullets)).c_str());
			//std::cout << _fps << " with " << Global::registry.view<entt::tag<"Player"_hs>>().size() << " and " << Global::registry.view<entt::tag<"Enemy"_hs>>().size() << "\n";
		} else {
			loop++;
		}
		_fps = _fpsLimiter.end();
	}
}

void MainGame::processInput() {
	SDL_Event evnt;
	while (SDL_PollEvent(&evnt)) {
		switch (evnt.type) {
		case SDL_QUIT:
			_gameState = GameState::EXIT;
			break;
		case SDL_MOUSEMOTION:
			_inputManager.setMouseCoords(evnt.motion.x, evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			_inputManager.pressKey(evnt.key.keysym.sym);
			break;
		case SDL_KEYUP:
			_inputManager.releaseKey(evnt.key.keysym.sym);
			break;
		case SDL_MOUSEBUTTONDOWN:
			_inputManager.pressKey(evnt.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			_inputManager.releaseKey(evnt.button.button);
			break;
		}
	}
}

void MainGame::drawGame() {
	// Set the base depth to 1.0
	glClearDepth(1.0);
	// Clear the color and depth buffer
	glClearColor(0., 0., 0., 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniform1i(texLoc, 0);

	// Grab the camera matrix
	glm::mat4 projectionMatrix = _camera.getCameraMatrix();
	glUniformMatrix4fv(camLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
	_systems.drawSprites(&_batch);
	_window.swapBuffer();
}