#version 450 core 

layout (location = 0) in vec4 vPos;

void main()
{
	gl_position = vPos;
}