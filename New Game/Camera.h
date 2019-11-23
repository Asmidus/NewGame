#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



class Camera {
public:
	Camera();
	Camera(int w, int h, float x = 0, float y = 0);
	~Camera();

	//sets up the orthographic matrix and screen dimensions
	void init(int screenWidth, int screenHeight);

	//updates the camera matrix if needed
	void update();

	glm::vec2 convertScreenToWorld(glm::vec2 screenCoords);

	//bool isBoxInView(const glm::vec2 & position, const glm::vec2 & dimensions);

	//void offsetPosition(const glm::vec2 & offset) { _position += offset; _needsMatrixUpdate = true; }
	//void offsetScale(float offset) { _scale += offset; if (_scale < 0.001f) _scale = 0.001f; _needsMatrixUpdate = true; }

	//setters
	void setPosition(const glm::vec2 & newPosition) { _position = newPosition; _needsMatrixUpdate = true; }
	void setScale(float newScale) { _scale = newScale; _needsMatrixUpdate = true; }

	//getters
	const glm::vec2& getPosition() const { return _position; }
	float getScale() const { return _scale; }
	const glm::mat4& getCameraMatrix() const { return _cameraMatrix; }
	float getAspectRatio() const { return (float)_screenWidth / (float)_screenHeight; }

	void view() { glViewport(0, 0, _screenWidth, _screenHeight); }

private:
	int _screenWidth, _screenHeight;
	bool _needsMatrixUpdate;
	float _scale;
	glm::vec2 _position;
	glm::mat4 _cameraMatrix;
	glm::mat4 _orthoMatrix;
};

