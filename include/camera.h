#pragma once

#include <stdio.h>
#include <glm/glm.hpp>

struct SCamera
{
	enum Camera_Movement
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT
	};

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;

	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	const float MovementSpeed = .5f;
	float MouseSensitivity = 1.f;



};


void InitCamera(SCamera& in)
{
	in.Front = glm::vec3(0.0f, 0.0f, -1.0f);
	in.Position = glm::vec3(0.0f, 0.0f, 0.0f);
	in.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	in.WorldUp = in.Up;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	in.Yaw = -90.f;
	in.Pitch = 0.f;
}

float cam_dist = 2.f;

void MoveAndOrientCamera(SCamera& in, glm::vec3 target, float distance, float xoffset, float yoffset)
{
	in.Yaw -= xoffset; //* in.MovementSpeed;
	in.Pitch -= yoffset; //* in.MovementSpeed;

	if (in.Pitch > 89.0f)
		in.Pitch = 89.0f;
	if (in.Yaw < -89.f)
		in.Yaw = -89.0f;

	in.Position = glm::vec3(cos(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch)),
		sin(glm::radians(in.Pitch)),
		sin(glm::radians(in.Yaw)) * cos(glm::radians(in.Pitch)));

	in.Position *= distance;

	// get vector from camera position to target vector
	in.Front = glm::normalize(target - in.Position);

	// get direction to move camera on x axis around the side of sphere
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	// get direction to move camera on y axis towards top pole
	in.Up = glm::normalize(glm::cross(in.Right, in.Front));

}