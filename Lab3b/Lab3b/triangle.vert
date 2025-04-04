#version 450 core

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vCol;

out vec3 col;

uniform mat4 transformations;

void main() 
{
	gl_Position = transformations * vPos;
	col = vCol;
}