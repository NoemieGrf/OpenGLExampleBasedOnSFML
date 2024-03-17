#pragma once

#include <glm/glm.hpp>

// lighting
inline glm::vec3 gLightPos[] = {
    glm::vec3(1.0f, 0.2f, -0.5f),
    glm::vec3(-1.0f, -0.4f, -0.5f),
    glm::vec3(0.8f, -0.2f, 0.0f),
    glm::vec3(2.0f, 2.0f, 0.5f),
    glm::vec3(2.0f, -2.0f, 0.5f),
};

inline glm::vec3 gLightColor[] = {
    glm::vec3(0.4f, 0.8f, 0.1f),
    glm::vec3(1.0f, 0.1f, 0.2f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.8f, 0.3f, 0.1f),
    glm::vec3(0.6f, 0.4f, 0.3f),
};
