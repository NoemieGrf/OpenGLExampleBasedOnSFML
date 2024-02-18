#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 position_, float pitch_, float yaw_, glm::vec3 world_up_);

public:
	glm::mat4 getViewMatrix();
	void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

private:
	void updateCameraVectors();

private:
	// Camera options
	float MovementSpeed = 2.5f;
	float MouseSensitivity = 0.01f;
	float Zoom = 45.0f;

public:
	glm::vec3 Position;
	glm::vec3 WorldUp;

	glm::vec3 Forward;
	glm::vec3 Right;
	glm::vec3 Up;

	float Pitch;
	float Yaw;
};
