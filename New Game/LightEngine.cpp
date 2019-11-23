#include "LightEngine.h"
#include "TextureManager.h"
#include "Vertex.h"
#include "Sprite.h"
#include "Light.h"
#include "Transform.h"
#include <iostream>



LightEngine::LightEngine(void) {
	_vbo = 0;
}

LightEngine::~LightEngine(void) {}

void LightEngine::LoadShaders(void) {
	hullShader.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
	hullShader.addAttribute("vertexPosition");
	hullShader.addAttribute("vertexColor");
	hullShader.addAttribute("vertexUV");
	hullShader.link();
	hPos = hullShader.getUniformLocation("P");

	distortShader.compileShaders("Shaders/textureShading.vert", "Shaders/distort.fx");
	distortShader.addAttribute("vertexPosition");
	distortShader.addAttribute("vertexColor");
	distortShader.addAttribute("vertexUV");
	distortShader.link();
	dPos = distortShader.getUniformLocation("P");

	reduceShader.compileShaders("Shaders/textureShading.vert", "Shaders/reduce.fx");
	reduceShader.addAttribute("vertexPosition");
	reduceShader.addAttribute("vertexColor");
	reduceShader.addAttribute("vertexUV");
	reduceShader.link();
	rPos = reduceShader.getUniformLocation("P");

	shadowShader.compileShaders("Shaders/textureShading.vert", "Shaders/shadow.fx");
	shadowShader.addAttribute("vertexPosition");
	shadowShader.addAttribute("vertexColor");
	shadowShader.addAttribute("vertexUV");
	shadowShader.link();
	sPos = shadowShader.getUniformLocation("P");

	blurVShader.compileShaders("Shaders/textureShading.vert", "Shaders/blurV.fx");
	blurVShader.addAttribute("vertexPosition");
	blurVShader.addAttribute("vertexColor");
	blurVShader.addAttribute("vertexUV");
	blurVShader.link();
	bvPos = blurVShader.getUniformLocation("P");

	blurHShader.compileShaders("Shaders/textureShading.vert", "Shaders/blurH.fx");
	blurHShader.addAttribute("vertexPosition");
	blurHShader.addAttribute("vertexColor");
	blurHShader.addAttribute("vertexUV");
	blurHShader.link();
	bhPos = blurHShader.getUniformLocation("P");
	_camera.init(1, 1);
	_batch.init();
}

void LightEngine::Begin(const Light& light,  const Transform& transform) {
	unsigned int size = light.resolution;
	hullShader.use();
	_camera.init(size, size);
	_camera.setScale(1/light.radius);
	float x = transform.rect.x + transform.rect.w * light.pos.x,
		y = transform.rect.y + transform.rect.h * light.pos.y;
	glm::vec2 point = glm::vec2(x, y);
	float angle = transform.angle;
	auto center = transform.center * glm::vec2(transform.rect.w, transform.rect.h) + glm::vec2(transform.rect.x, transform.rect.y);
	x = cos(angle) * (point.x - center.x) - sin(angle) * (point.y - center.y) + center.x;
	y = sin(angle) * (point.x - center.x) + cos(angle) * (point.y - center.y) + center.y;
	_camera.setPosition(glm::vec2(x,
								  size - y));
	_camera.update();
	_camera.view();
	auto& matrix = _camera.getCameraMatrix();
	glUniformMatrix4fv(hPos, 1, GL_FALSE, &matrix[0][0]);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto info = TextureManager::GetRenderTextures(size);
	glBindFramebuffer(GL_FRAMEBUFFER, info[0].first);
	_batch.begin(GlyphSortType::TEXTURE);
}

void LightEngine::End() {
	_batch.end();
	_batch.renderBatch();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	hullShader.unuse();
}

glm::vec2 rotatePoint(const glm::vec2& pos, float angle) {
	glm::vec2 newv;
	newv.x = pos.x * cos(angle) - pos.y * sin(angle);
	newv.y = pos.x * sin(angle) + pos.y * cos(angle);
	return newv;
}

void LightEngine::DrawHull(Light* light, Transform* lightTransform, Sprite* sprite, Transform* transform) {
	glm::vec4 t = glm::vec4(transform->rect.x, light->resolution - transform->rect.y - transform->rect.h, transform->rect.w, transform->rect.h);
	glm::vec4 u = sprite->getUV();
	if (transform->angle) {
		_batch.draw(t, u, sprite->texture, transform->z, sprite->color, -transform->angle, transform->center);
	} else {
		_batch.draw(t, u, sprite->texture, transform->z, sprite->color);
	}
}

void LightEngine::Draw(GLuint& fbo, Light* light, GLuint texture) {
	if (_vbo == 0) {
		glGenBuffers(1, &_vbo);
	}
	static Vertex vertexData[6];
	int size = light->resolution;
	int width = size, height = size;
	int x = 0, y = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	vertexData[0].setPosition(x + width, y + height);
	vertexData[0].setUV(1.0f, 0.0f);
	vertexData[1].setPosition(x, y + height);
	vertexData[1].setUV(0.0f, 0.0f);
	vertexData[2].setPosition(x, y);
	vertexData[2].setUV(0.0f, 1.0f);
	vertexData[3].setPosition(x, y);
	vertexData[3].setUV(0.0f, 1.0f);
	vertexData[4].setPosition(x + width, y);
	vertexData[4].setUV(1.0f, 1.0f);
	vertexData[5].setPosition(x + width, y + height);
	vertexData[5].setUV(1.0f, 0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LightEngine::Draw(Light* light, Transform* lightTransform) {
	if (_vbo == 0) {
		glGenBuffers(1, &_vbo);
	}
	static Vertex vertexData[6];
	int size = light->resolution;
	int dim = size * light->radius;
	int x = lightTransform->rect.x + lightTransform->rect.w * light->pos.x,
		y = lightTransform->rect.y + lightTransform->rect.h * light->pos.y;
	glm::vec2 point = glm::vec2(x, y);
	float angle = lightTransform->angle;
	auto center = lightTransform->center * glm::vec2(lightTransform->rect.w, lightTransform->rect.h) + glm::vec2(lightTransform->rect.x, lightTransform->rect.y);
	x = cos(angle) * (point.x - center.x) - sin(angle) * (point.y - center.y) + center.x - dim / 2;
	y = sin(angle) * (point.x - center.x) + cos(angle) * (point.y - center.y) + center.y - dim / 2;
	vertexData[0].setPosition(x + dim, y + dim);
	vertexData[0].setUV(1.0f, 1.0f);
	vertexData[1].setPosition(x, y + dim);
	vertexData[1].setUV(0.0f, 1.0f);
	vertexData[2].setPosition(x, y);
	vertexData[2].setUV(0.0f, 0.0f);
	vertexData[3].setPosition(x, y);
	vertexData[3].setUV(0.0f, 0.0f);
	vertexData[4].setPosition(x + dim, y);
	vertexData[4].setUV(1.0f, 0.0f);
	vertexData[5].setPosition(x + dim, y + dim);
	vertexData[5].setUV(1.0f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindTexture(GL_TEXTURE_2D, light->shadowTex);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LightEngine::CreateShadows(Light* light) {
	_camera.setScale(1.0f);
	_camera.setPosition(glm::vec2(light->resolution / 2, light->resolution / 2));
	_camera.update();
	auto info = TextureManager::GetRenderTextures(light->resolution);
	auto& matrix = _camera.getCameraMatrix();

	distortShader.use();
	glUniformMatrix4fv(dPos, 1, GL_FALSE, &matrix[0][0]);
	Draw(info[1].first, light, info[0].second);
	distortShader.unuse();

	reduceShader.use();
	static GLuint reduceRenderTargetSize = reduceShader.getUniformLocation("renderTargetSize");
	glUniformMatrix4fv(rPos, 1, GL_FALSE, &matrix[0][0]);
	glUniform1f(reduceRenderTargetSize, light->resolution);
	Draw(info[2].first, light, info[1].second);
	reduceShader.unuse();

	shadowShader.use();
	static GLuint reduce = shadowShader.getUniformLocation("reduce");
	static GLuint renderTargetSize = shadowShader.getUniformLocation("renderTargetSize");
	static GLuint lightColor = shadowShader.getUniformLocation("lightColor");
	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	glBindTexture(GL_TEXTURE_2D, info[2].second);
	glActiveTexture(GL_TEXTURE0);
	static GLfloat col[4] = { 0, 0, 0, 1 };
	col[0] = light->color[0];
	col[1] = light->color[1];
	col[2] = light->color[2];
	glUniformMatrix4fv(sPos, 1, GL_FALSE, &matrix[0][0]);
	glUniform1i(reduce, 1);
	glUniform1i(renderTargetSize, light->resolution);
	glUniform4fv(lightColor, 1, col);
	TextureManager::ClearTexture(light->shadowFBO);
	Draw(light->shadowFBO, light, info[0].second);
	shadowShader.unuse();

	glBlendFunc(GL_ONE, GL_ONE);

	blurHShader.use();
	glUniformMatrix4fv(bhPos, 1, GL_FALSE, &matrix[0][0]);
	TextureManager::ClearTexture(info[3].first);
	Draw(info[3].first, light, light->shadowTex);
	blurHShader.unuse();

	blurVShader.use();
	glUniformMatrix4fv(bvPos, 1, GL_FALSE, &matrix[0][0]);
	TextureManager::ClearTexture(light->shadowFBO);
	Draw(light->shadowFBO, light, info[3].second);
	blurVShader.unuse();

	TextureManager::ClearTexture(info[0].first);
}