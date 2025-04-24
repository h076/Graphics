#pragma once

#include <stdio.h>
#include <glm/glm.hpp>

struct SpotLight {
	glm::vec3 Dir;
	glm::vec3 Pos;
	glm::vec3 Col;

	float CutOffAngle;
};

void InitSpotLight(SpotLight& in, glm::vec3 pos, glm::vec3 dir, glm::vec3 col, float cutOff) {
	in.Pos = pos;
	in.Dir = dir;
	in.Col = col;

	in.CutOffAngle = cutOff;
}

void MoveSpotLight(SpotLight& in, glm::vec3 pos) {
	in.Pos = pos;
}

void ChangeSpotLightDirection(SpotLight& in, glm::vec3 dir) {
	in.Dir = dir;
}

void ChangeSpotLightCutOff(SpotLight& in, float cutOff) {
	in.CutOffAngle = cutOff;
}