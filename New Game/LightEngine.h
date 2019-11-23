#pragma once
#include "Program.h"
#include "Camera.h"
#include "SpriteBatch.h"
#include <gl/glew.h>
#include <memory>
#include <list>
#include <map>
#include <vector>

class Light;
class Sprite;
class Transform;
class LightEngine {
public:
	LightEngine();
	virtual ~LightEngine();

	void LoadShaders();
	void Begin(const Light& light, const Transform& transform);
	void End();
	void DrawHull(Light* light, Transform* lightTransform, Sprite* sprite, Transform* transform);
	void Draw(GLuint& fbo, Light* light, GLuint texture);
	void Draw(Light* light, Transform* lightTransform);
	void CreateShadows(Light* light);
	void UpdateMatrix(glm::mat4 mat);

	void Clean();

	// Shaders
	Program hullShader;
	Program distortShader;
	Program reduceShader;
	Program shadowShader;
	Program blurVShader;
	Program blurHShader;
	Camera _camera;
	SpriteBatch _batch;

	GLuint hPos;
	GLuint dPos;
	GLuint rPos;
	GLuint sPos;
	GLuint bvPos;
	GLuint bhPos;

	GLuint _vbo;
	glm::mat4 _matrix;
};

