#pragma once

#include <array>
#include <glm/glm.hpp>

inline std::array<float, 30> gTransparentWindowVertices = {
     // positions         // uv
    -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
     1.0f, -1.0f,  0.0f,  1.0f,  1.0f,

    -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  0.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  0.0f,  1.0f,  0.0f
};

inline std::array<glm::vec3, 5> gTransparentWindowPosition = {
    glm::vec3(1.5f, 1.0f, 1.5f),
    glm::vec3(-1.9f, 1.0f, -1.1f),
    glm::vec3(1.0f, 1.0f, -2.0f),
    glm::vec3(-1.0f, 1.0f, -2.9f),
    glm::vec3(-3.0f, 1.0f, -2.9f)
};