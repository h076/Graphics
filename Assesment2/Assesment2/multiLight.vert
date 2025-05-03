#version 450 core

#define NUM_LIGHTS 5

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vNor;
layout (location = 2) in vec2 vUV;

layout (location = 3) in vec4 vCol;

out vec2 TexCoord;
out vec3 nor;
out vec3 FragPosWorldSpace;
out vec4 FragPosProjectedLightSpace[NUM_LIGHTS];
out vec4 col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 projectedLightSpaceMatrix[NUM_LIGHTS];

void main() {
	gl_Position = projection * view * model * vPos;
	TexCoord = vUV;
	col = vCol;
	nor = mat3(transpose(inverse(model))) * vNor;
	FragPosWorldSpace = vec3(model * vPos);
	for(int i=0; i<NUM_LIGHTS; i++) {
		FragPosProjectedLightSpace[i] = projectedLightSpaceMatrix[i] * model * vPos;
	}
}