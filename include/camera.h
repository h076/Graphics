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

	const float MovementSpeed = 10.f;
	float MouseSensitivity = 1.f;

};


void InitCamera(SCamera& in, glm::vec3 pos)
{
	in.Front = glm::vec3(0.0f, 0.0f, -1.0f);
	in.Position = pos;
	in.Up = glm::vec3(0.0f, 1.0f, 0.0f);
	in.WorldUp = in.Up;
	in.Right = glm::normalize(glm::cross(in.Front, in.WorldUp));

	in.Yaw = -90.f;
	in.Pitch = 0.f;
}

float cam_dist = 2.f;

void UpdateCameraVectors(SCamera& camera)
{
	glm::vec3 front;
	front.x = cos(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	front.y = sin(glm::radians(camera.Pitch));
	front.z = sin(glm::radians(camera.Yaw)) * cos(glm::radians(camera.Pitch));
	camera.Front = glm::normalize(front);
	camera.Right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
	camera.Up = glm::normalize(glm::cross(camera.Right, camera.Front));
}

void MoveAndOrientCamera(SCamera& camera, float xoffset, float yoffset)
{
	// Adjust yaw and pitch
	camera.Yaw += xoffset * camera.MouseSensitivity;
	camera.Pitch += yoffset * camera.MouseSensitivity;

	// Constrain pitch
	if (camera.Pitch > 89.0f)  camera.Pitch = 89.0f;
	if (camera.Pitch < -89.0f) camera.Pitch = -89.0f;

	// Recompute direction vectors
	UpdateCameraVectors(camera);
}