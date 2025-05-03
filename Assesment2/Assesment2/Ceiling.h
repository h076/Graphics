#pragma once

#include "objModel.h"

class Ceiling : public ObjModel {
public:
	Ceiling(unsigned int vao, unsigned int vbo) : ObjModel(vao, vbo) {}

	void draw(unsigned int shader, const glm::vec3& camPos,
		const glm::vec3& camFront, const glm::vec3& camUp) const;
};