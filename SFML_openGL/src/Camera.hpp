#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
	using vec3 = glm::vec3;
	using mat4 = glm::mat4;
public:
	vec3 Position;
	vec3 WorldUp;
	
	vec3 Forward;
	vec3 Right;
	vec3 Up;
	
	float Pitch;
	float Yaw;
	
public:

	Camera(vec3 position_, float pitch_, float yaw_, vec3 world_up_)
		: Position(position_)
		, WorldUp(world_up_)
		, Forward(vec3{
			glm::cos(pitch_) * glm::sin(yaw_),
			glm::sin(pitch_),
			glm::cos(pitch_) * glm::cos(yaw_)})
		, Right(glm::normalize(glm::cross(Forward, WorldUp)))
		, Up(glm::normalize(glm::cross(Forward, Right)))
		, Pitch(pitch_)
		, Yaw(yaw_)
	{
	}

	auto getViewMatrix() -> mat4
	{
		return glm::lookAt(Position, Position + Forward, WorldUp);
	}

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

private:
	// Camera options
	float MovementSpeed = 2.5f;
	float MouseSensitivity = 0.01f;
	float Zoom = 45.0f;

	void updateCameraVectors()
	{
		// Calculate the new Front vector
		Forward = glm::normalize(vec3{
			glm::cos(Pitch) * glm::sin(Yaw),
			glm::sin(Pitch),
			glm::cos(Pitch) * glm::cos(Yaw) });
		// Also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Forward, WorldUp));  
		Up = glm::normalize(glm::cross(Right, Forward));
	}
};
