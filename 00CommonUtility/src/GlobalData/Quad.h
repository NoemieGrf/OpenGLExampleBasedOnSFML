#pragma once

#include <array>

inline std::array<float, 30> gQuadVertices = {
    // positions          // uv
    -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  0.0f,  1.0f,  1.0f,

    -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
};