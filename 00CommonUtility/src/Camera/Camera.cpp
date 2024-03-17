#include "Camera.h"

Camera::Camera(glm::vec3 position, float pitch, float yaw, const glm::vec3& worldUp)
	: _position(position)
	, _worldUp(worldUp)
	, _forward(glm::vec3{
		glm::cos(pitch) * glm::sin(yaw),
		glm::sin(pitch),
		glm::cos(pitch) * glm::cos(yaw) })
	, _right(glm::normalize(glm::cross(_forward, _worldUp)))
	, _up(glm::normalize(glm::cross(_forward, _right)))
	, _pitch(pitch)
	, _yaw(yaw)
{
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(_position, _position + _forward, _worldUp);
}

const glm::vec3& Camera::GetPosition()
{
	return _position;
}
