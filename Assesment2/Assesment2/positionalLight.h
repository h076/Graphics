#pragma once

#include <stdio.h>
#include <glm/glm.hpp>

struct PosLight {
	glm::vec3 Dir;
	glm::vec3 Pos;
	glm::vec3 Col;
};

void InitPosLight(PosLight& in, glm::vec3 pos, glm::vec3 dir, glm::vec3 col) {
	in.Pos = pos;
	in.Dir = dir;
	in.Col = col;
}

void MovePosLight(PosLight& in, glm::vec3 pos) {
	in.Pos = pos;
}

void ChangePosLightDirection(PosLight& in, glm::vec3 dir) {
	in.Dir = dir;
}
