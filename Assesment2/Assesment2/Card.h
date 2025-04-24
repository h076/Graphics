#pragma once

#include "objModel.h"

class Card : public ObjModel {
public:
	Card(unsigned int vao, unsigned int vbo) : ObjModel(vao, vbo) {}

	void draw(unsigned int shader, const glm::vec3& camPos,
		const glm::vec3& camFront, const glm::vec3& camUp,
		glm::vec3 cardPosition, float rotation) const;
};