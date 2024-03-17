#pragma once

#include <array>

inline std::array<float, 48> gPlaneVertices =
{
    // positions(model position)  // normals         // uv
     5.0f, -0.5f,  5.0f,          0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,          0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,          0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
     5.0f, -0.5f,  5.0f,          0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,          0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
     5.0f, -0.5f, -5.0f,          0.0f, 1.0f, 0.0f,  1.0f, 1.0f
};