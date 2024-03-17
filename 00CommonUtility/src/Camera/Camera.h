#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 position, float pitch, float yaw, const glm::vec3& worldUp);

public:
	glm::mat4 GetViewMatrix();
	const glm::vec3& GetPosition();

public:
	glm::vec3 _position;
	glm::vec3 _worldUp;

	glm::vec3 _forward;
	glm::vec3 _right;
	glm::vec3 _up;

	float _pitch;
	float _yaw;
};
