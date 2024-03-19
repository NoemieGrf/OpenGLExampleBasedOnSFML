#pragma once

#include <array>
#include <glm/glm.hpp>

inline std::array<float, 30> gTransparentWindowVertices = {
    // positions         // uv
    0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
    0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
    1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

    0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
    1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
    1.0f,  0.5f,  0.0f,  1.0f,  0.0f
};

inline std::array<glm::vec3, 3> gTransparentWindowPosition = {
    glm::vec3(1.9f, 0.5f, 0.1f),
    glm::vec3(0.1f, 0.5f, 0.1f),
    glm::vec3(0.1f, 0.5f, 1.9f)
};