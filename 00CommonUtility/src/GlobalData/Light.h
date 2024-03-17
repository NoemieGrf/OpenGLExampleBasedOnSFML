#pragma once

#include <array>
#include <glm/glm.hpp>

inline glm::vec3 gSingleLightPosition = glm::vec3(-0.3f, 1.5f, -2.5f);

inline std::array<glm::vec3, 5> gMultiLightPosition = {
	glm::vec3(-0.3f, 1.5f, -2.5f),
	glm::vec3(-0.3f, 1.5f, -2.5f),
	glm::vec3(-0.3f, 1.5f, -2.5f),
	glm::vec3(-0.3f, 1.5f, -2.5f),
	glm::vec3(-0.3f, 1.5f, -2.5f)
};