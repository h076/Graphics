#pragma once

#include "objModel.h"

class Chair : public ObjModel {
	public:
		Chair(unsigned int vao, unsigned int vbo) : ObjModel(vao, vbo) {}

		void draw(unsigned int shader, const glm::vec3& camPos,
			const glm::vec3& camFront, const glm::vec3& camUp) const;
};