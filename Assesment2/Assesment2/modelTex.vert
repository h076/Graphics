#version 450 core

layout(location = 0) in vec4 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out vec2 TexCoord;
out vec3 nor;
out vec3 FragPosWorldSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * aPos;
	TexCoord = aUV;
	nor = mat3(transpose(inverse(model))) * aNormal;
	FragPosWorldSpace = vec3(model * aPos);
}