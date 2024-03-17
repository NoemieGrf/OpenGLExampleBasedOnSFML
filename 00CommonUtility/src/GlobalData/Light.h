#pragma once

#include <array>
#include <glm/glm.hpp>

inline glm::vec3 gSingleLightPosition = glm::vec3(-0.3f, 1.5f, -2.5f);

inline std::array<glm::vec3, 5> gMultiLightPosition = {
	glm::vec3(-0.3f, 0.5f, -2.5f),
	glm::vec3(-1.3f, 0.5f, -1.5f),
	glm::vec3(-3.3f, 0.5f, 1.5f),
	glm::vec3(-2.3f, 0.5f, -2.5f),
	glm::vec3(0.7f, 0.5f, 1.5f)
};

inline std::array<glm::vec3, 5> gMultiLightColor = {
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 1.0f),
};