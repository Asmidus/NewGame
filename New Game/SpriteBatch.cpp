#include "SpriteBatch.h"


#include "SpriteBatch.h"

#include <algorithm>



Glyph::Glyph(const glm::vec4& destRect, const glm::vec4& uvRect,
			 GLuint Texture, float Depth, const Color& color) : texture(Texture), depth(Depth) {
	topLeft.color = color;
	topLeft.pos = glm::vec2(destRect.x, destRect.y + destRect.w);
	topLeft.uv = glm::vec2(uvRect.x, uvRect.y + uvRect.w);

	bottomLeft.color = color;
	bottomLeft.pos = glm::vec2(destRect.x, destRect.y);
	bottomLeft.uv = glm::vec2(uvRect.x, uvRect.y);

	bottomRight.color = color;
	bottomRight.pos = glm::vec2(destRect.x + destRect.z, destRect.y);
	bottomRight.uv = glm::vec2(uvRect.x + uvRect.z, uvRect.y);

	topRight.color = color;
	topRight.pos = glm::vec2(destRect.x + destRect.z, destRect.y + destRect.w);
	topRight.uv = glm::vec2(uvRect.x + uvRect.z, uvRect.y + uvRect.w);
}

Glyph::Glyph(const glm::vec4& destRect, const glm::vec4& uvRect,
			 GLuint Texture, float Depth, const Color& color,
			 float angle, const glm::vec2& center) : texture(Texture), depth(Depth) {
	static glm::vec2 tlDims, brDims, tl, bl, br, tr;
	tlDims.x = -destRect.z * center.x;
	tlDims.y = -destRect.w * center.y;
	brDims.x = destRect.z * (1 - center.x);
	brDims.y = destRect.w * (1 - center.y);

	// Get points centered around origin
	tl.x = tlDims.x;
	tl.y = tlDims.y;

	bl.x = tlDims.x;
	bl.y = brDims.y;

	br.x = brDims.x;
	br.y = brDims.y;

	tr.x = brDims.x;
	tr.y = tlDims.y;

	// Rotate the points
	tl = rotatePoint(tl, angle) - tlDims;
	bl = rotatePoint(bl, angle) - tlDims;
	br = rotatePoint(br, angle) - tlDims;
	tr = rotatePoint(tr, angle) - tlDims;

	topLeft.color = color;
	topLeft.pos.x = destRect.x + tl.x;
	topLeft.pos.y = destRect.y + tl.y;
	topLeft.uv.x = uvRect.x;
	topLeft.uv.y = uvRect.y + uvRect.w;

	bottomLeft.color = color;
	bottomLeft.pos.x = destRect.x + bl.x;
	bottomLeft.pos.y = destRect.y + bl.y;
	bottomLeft.uv.x = uvRect.x;
	bottomLeft.uv.y = uvRect.y;

	bottomRight.color = color;
	bottomRight.pos.x = destRect.x + br.x;
	bottomRight.pos.y = destRect.y + br.y;
	bottomRight.uv.x = uvRect.x + uvRect.z;
	bottomRight.uv.y = uvRect.y;

	topRight.color = color;
	topRight.pos.x = destRect.x + tr.x;
	topRight.pos.y = destRect.y + tr.y;
	topRight.uv.x = uvRect.x + uvRect.z;
	topRight.uv.y = uvRect.y + uvRect.w;
}

glm::vec2 Glyph::rotatePoint(const glm::vec2& pos, float angle) {
	glm::vec2 newv;
	newv.x = pos.x * cos(angle) - pos.y * sin(angle);
	newv.y = pos.x * sin(angle) + pos.y * cos(angle);
	return newv;
}

SpriteBatch::SpriteBatch() : _vbo(0), _vao(0) {}

SpriteBatch::~SpriteBatch() {}

void SpriteBatch::init() {
	createVertexArray();
	static int maxBatchSize = 1000000 * 3 / sizeof(Vertex); //Cap of 3mb
	_vertices.resize(maxBatchSize);
}

void SpriteBatch::dispose() {
	for(auto& vao : _vao) {
		glDeleteVertexArrays(1, &vao);
	}
	for(auto& vbo : _vbo) {
		glDeleteBuffers(1, &vbo);
	}
}

void SpriteBatch::begin(GlyphSortType sortType /* GlyphSortType::TEXTURE */) {
	_sortType = sortType;
	_renderBatches.clear();
	currVBO = 0;

	// Makes _glpyhs.size() == 0, however it does not free internal memory.
	// So when we later call emplace_back it doesn't need to internally call new.
	_glyphs.clear();
}

void SpriteBatch::end() {
	// Set up all pointers for fast sorting
	_glyphPointers.resize(_glyphs.size());
	for (size_t i = 0; i < _glyphs.size(); i++) {
		_glyphPointers[i] = &_glyphs[i];
	}

	sortGlyphs();
	createRenderBatches();
}

void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect,
					   GLuint texture, float depth, const Color& color) {
	_glyphs.emplace_back(destRect, uvRect, texture, depth, color);
}

void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect,
					   GLuint texture, float depth, const Color& color, float angle, const glm::vec2& center) {
	_glyphs.emplace_back(destRect, uvRect, texture, depth, color, angle, center);
}

void SpriteBatch::draw(const glm::vec4& destRect, const glm::vec4& uvRect,
					   GLuint texture, float depth, const Color& color, const glm::vec2& dir) {
	const glm::vec2 right(1.0f, 0.0f);
	float angle = acos(glm::dot(right, dir));
	if (dir.y < 0.0f) angle = -angle;

	_glyphs.emplace_back(destRect, uvRect, texture, depth, color, angle);
}

void SpriteBatch::renderBatch() {

	// Bind our VAO. This sets up the opengl state we need, including the 
	// vertex attribute pointers and it binds the VBO
	int currVAO = 0;
	for (size_t i = 0; i < _renderBatches.size(); i++) {
		if (_renderBatches[i].offset == 0) {
			glBindVertexArray(0);
			glBindVertexArray(_vao[currVAO++]);
		}
		glBindTexture(GL_TEXTURE_2D, _renderBatches[i].texture);

		glDrawArrays(GL_TRIANGLES, _renderBatches[i].offset, _renderBatches[i].numVertices);
	}
	glBindVertexArray(0);

}

void SpriteBatch::renderCurrentBatch(int numVertices) {
	if (currVBO >= _vao.size()) {
		createVertexArray();
	}

	// Bind our VBO
	glBindBuffer(GL_ARRAY_BUFFER, _vbo[currVBO++]);
	// Orphan the buffer (for speed)
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), 0, GL_DYNAMIC_DRAW);
	// Upload the data
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vertex), _vertices.data());

	// Unbind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Bind our VAO. This sets up the opengl state we need, including the 
	// vertex attribute pointers and it binds the VBO
	//glBindVertexArray(_vao.back());

	//glBindTexture(GL_TEXTURE_2D, _renderBatches[0].texture);

	//glDrawArrays(GL_TRIANGLES, 0, _renderBatches[0].numVertices);

	//glBindVertexArray(0);
}

void SpriteBatch::createRenderBatches() {
	// This will store all the vertices that we need to upload
	// Resize the buffer to the exact size we need so we can treat
	// it like an array
	static int maxBatchSize = 1000000 * 0.24 / sizeof(Vertex); //Cap of 1.2mb
	unsigned int numVertices = 6*_glyphPointers.size();

	if (_glyphPointers.empty()) {
		return;
	}

	size_t cg = 0;
	while (cg < _glyphPointers.size()) {
		int cv = 0; // current vertex
		int offset = 0; // current offset

		//Add the first batch
		_renderBatches.emplace_back(offset, 6, _glyphPointers[cg]->texture);
		_vertices[cv++] = _glyphPointers[cg]->topLeft;
		_vertices[cv++] = _glyphPointers[cg]->bottomLeft;
		_vertices[cv++] = _glyphPointers[cg]->bottomRight;
		_vertices[cv++] = _glyphPointers[cg]->bottomRight;
		_vertices[cv++] = _glyphPointers[cg]->topRight;
		_vertices[cv++] = _glyphPointers[cg++]->topLeft;
		offset++;

		//Add all the rest of the glyphs
		while (cg < _glyphPointers.size() and cv < maxBatchSize) {

			// Check if this glyph can be part of the current batch
			if (_glyphPointers[cg]->texture != _glyphPointers[cg - 1]->texture) {
				// Make a new batch
				_renderBatches.emplace_back(6 * offset, 6, _glyphPointers[cg]->texture);
			} else {
				// If its part of the current batch, just increase numVertices
				_renderBatches.back().numVertices += 6;
			}
			_vertices[cv++] = _glyphPointers[cg]->topLeft;
			_vertices[cv++] = _glyphPointers[cg]->bottomLeft;
			_vertices[cv++] = _glyphPointers[cg]->bottomRight;
			_vertices[cv++] = _glyphPointers[cg]->bottomRight;
			_vertices[cv++] = _glyphPointers[cg]->topRight;
			_vertices[cv++] = _glyphPointers[cg++]->topLeft;
			offset++;
		}
		renderCurrentBatch(cv);
	}
}

void SpriteBatch::createVertexArray() {

	// Bind the VAO. All subsequent opengl calls will modify it's state.
	_vao.push_back(0);
	glGenVertexArrays(1, &_vao.back());
	glBindVertexArray(_vao.back());

	_vbo.push_back(0);
	glGenBuffers(1, &_vbo.back());
	glBindBuffer(GL_ARRAY_BUFFER, _vbo.back());

	//Tell opengl what attribute arrays we need
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	//This is the position attribute pointer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
	//This is the color attribute pointer
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));
	//This is the UV attribute pointer
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SpriteBatch::sortGlyphs() {

	switch (_sortType) {
	case GlyphSortType::BACK_TO_FRONT:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareBackToFront);
		break;
	case GlyphSortType::FRONT_TO_BACK:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareFrontToBack);
		break;
	case GlyphSortType::TEXTURE:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareTexture);
		break;
	case GlyphSortType::BTF_TEXTURE:
		std::stable_sort(_glyphPointers.begin(), _glyphPointers.end(), compareBackToFront);
	}
}

bool SpriteBatch::compareFrontToBack(Glyph* a, Glyph* b) {
	return (a->depth < b->depth);
}

bool SpriteBatch::compareBackToFront(Glyph* a, Glyph* b) {
	return (a->depth > b->depth);
}

bool SpriteBatch::compareTexture(Glyph* a, Glyph* b) {
	return (a->texture < b->texture);
}

bool SpriteBatch::compareBTFTexture(Glyph* a, Glyph* b) {
	bool c = compareBackToFront(a, b);
	if (c) return c;
	return compareTexture(a, b);
}