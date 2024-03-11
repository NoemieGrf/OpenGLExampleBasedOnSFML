#include "Camera.hpp"

Camera::Camera(glm::vec3 position_, float pitch_, float yaw_, glm::vec3 world_up_)
	: Position(position_)
	, WorldUp(world_up_)
	, Forward(glm::vec3{
		glm::cos(pitch_) * glm::sin(yaw_),
		glm::sin(pitch_),
		glm::cos(pitch_) * glm::cos(yaw_) })
		, Right(glm::normalize(glm::cross(Forward, WorldUp)))
	, Up(glm::normalize(glm::cross(Forward, Right)))
	, Pitch(pitch_)
	, Yaw(yaw_)
{
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(Position, Position + Forward, WorldUp);
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
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

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	Forward = glm::normalize(glm::vec3{
		glm::cos(Pitch) * glm::sin(Yaw),
		glm::sin(Pitch),
		glm::cos(Pitch) * glm::cos(Yaw) });
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Forward, WorldUp));
	Up = glm::normalize(glm::cross(Right, Forward));
}
