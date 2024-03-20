#pragma once

#include <array>
#include <glm/glm.hpp>

inline glm::vec3 gSingleLightPosition = glm::vec3(-0.3f, 0.2f, -2.5f);

inline std::array<glm::vec3, 5> gMultiLightPosition = {
	glm::vec3(2.3f, 0.8f, -2.5f),
	glm::vec3(-1.3f, 0.8f, -1.5f),
	glm::vec3(-3.3f, 0.8f, 1.5f),
	glm::vec3(-2.3f, 0.8f, -2.5f),
	glm::vec3(0.7f, 0.8f, 1.5f)
};

inline std::array<glm::vec3, 5> gMultiLightColor = {
	glm::vec3(0.9f, 0.1f, 0.1f),
	glm::vec3(0.1f, 0.9f, 0.1f),
	glm::vec3(0.1f, 0.1f, 0.9f),
	glm::vec3(0.9f, 0.1f, 0.9f),
	glm::vec3(0.8f, 0.8f, 0.1f),
};