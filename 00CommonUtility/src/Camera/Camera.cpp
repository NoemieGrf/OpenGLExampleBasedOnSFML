#include "Camera.h"

Camera::Camera(glm::vec3 position_, float pitch_, float yaw_, glm::vec3 world_up_)
	: _position(position_)
	, _worldUp(world_up_)
	, _forward(glm::vec3{
		glm::cos(pitch_) * glm::sin(yaw_),
		glm::sin(pitch_),
		glm::cos(pitch_) * glm::cos(yaw_) })
	, _right(glm::normalize(glm::cross(_forward, _worldUp)))
	, _up(glm::normalize(glm::cross(_forward, _right)))
	, _pitch(pitch_)
	, _yaw(yaw_)
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
