#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(glm::vec3 position_, float pitch_, float yaw_, glm::vec3 world_up_);

public:
	glm::mat4 GetViewMatrix();

public:
	glm::vec3 _position;
	glm::vec3 _worldUp;

	glm::vec3 _forward;
	glm::vec3 _right;
	glm::vec3 _up;

	float _pitch;
	float _yaw;
};
